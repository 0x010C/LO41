/*
	PROGRAMME :
		PeliKanban
	DEVELOPPEUR :
		Paul-Émile Bretegnier <paul-emile.bretegnier@utbm.fr>
		Antoine LAMIELLE <antoine.lamielle@utbm.fr>
	PROJECT START DATE :
		November, 29th 2014
	LICENCE :
		PeliKanban is a project realised as a part of the LO41-lessons at the UTBM school, dealing with interprocess communication.

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program; if not, write to the Free Software Foundation, Inc.,
		51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


/*
 * ----------------------------
 * INCLUDES
 * ----------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>


/*
 * ----------------------------
 * GLOBAL VARIABLES
 * ----------------------------
*/

int msgid;


/*
 * ----------------------------
 * STRUCTUReS
 * ----------------------------
*/

/*
 * struct node :
 * - Pointer on the client node							client<node*>
 * - Array of pointers on the suppliers nodes			suppliers<node**> 
 * - Deep of the node relative to the root node			deep<unsigned int>
 * - Number of suppliers								nbSuppliers<unsigned int>
 * - Internal workshop ID (IWID)						id<unsigned int>
 * - Process ID of the workshop attached to the node	pid<pid_t>
 */
typedef struct node node;
struct node
{
	node *client;
	node **suppliers;
	unsigned int deep;
	unsigned int nbSuppliers;
	unsigned int id;
	pid_t pid;
};

/*
 * enum req_t
 * All the possible requests types that the workshops can send to the other one
 */
enum req_t {
	REQ_NULL,
	REQ_SEND_TICKET,
	REQ_SEND_CONTAINER,
	REQ_PING,
	REQ_REPLY_PING,
	REQ_SHUTDOWN,
	REQ_CONFIRM_SHUTDOWN,
	REQ_INFORM_NB_IN_CONTAINER
};
typedef enum req_t req_t;

/*
 * struct Message :
 * - IWID of the message recipient			to<long>
 * - IWID of the message sender				from<long>
 * - Request type attached to the message	request<req_t>
 * - Value attached to the message			value<long>
 */
typedef struct Message Message;
struct Message
{
	long to;
	long from;
	req_t request;
	long value;
};


/*
 * ----------------------------
 * FUNCTIONS
 * ----------------------------
*/

/*
 * -------- peli_initIPC --------
 * Description : Create and initialise the IPC object
 * Parameters :
 * Return : nothing but set the global variable msgid
 * ------------------------------
 */
void peli_initIPC()
{
	msgid = msgget(ftok("PeliKanban", 42), 0750 | IPC_CREAT | IPC_EXCL);
}

/*
 * -------- peli_destroyIPC --------
 * Description : Destroy the IPC object
 * Parameters :
 * Return :
 * ---------------------------------
 */
void peli_destroyIPC()
{
	msgctl(msgid, IPC_RMID, NULL);
}

/*
 * -------- peli_sendIPC --------
 * Description : Send an IPC message
 * Parameters : - The IWID of the receiver			to<long>
 *              - The type of the request			request<req_t>
 *              - The value to join to the message	value<long>
 * Return :
 * ------------------------------
 */
void peli_sendIPC(long to, req_t request, long value)
{
	int i;

	Message toSend;

	toSend.from = 1;
	toSend.request = request;
	toSend.value = value;

	if(to < 0)
	{
		for(i=1; i<-to; i++) {
			toSend.to = i;
			msgsnd(msgid, &toSend, sizeof(Message) - sizeof(long), 0);
		}
	}
	else
	{
		toSend.to = to;
		msgsnd(msgid, &toSend, sizeof(Message) - sizeof(long), 0);
	}
}

/*
 * -------- peli_rcvIPC --------
 * Description : Check if there is one message for me, if not wait or pass depending of the value of the flag
 * Parameters : Flag passing to msgrcv, will specify for example if the process wait or not (IPC_NOWAIT) when there is no messages		flag<int>
 * Return : the message recieved, or message null if no message where get																letter<Message>
 * -----------------------------
 */
Message peli_rcvIPC(int flag)
{
	Message letter;
	letter.to = -1;
	letter.from = -1;
	letter.request = REQ_NULL;
	letter.value = -1;
	msgrcv(msgid, &letter, sizeof(Message) - sizeof(long), 1, flag);
	return letter;
}

/*
 * -------- peli_intToChar --------
 * Description : Convert an integer into a string
 * Parameters : the integer to convert			nb<int>
 * Return : the dynamicaly allocated string		result<char*>
 * --------------------------------
 */
char *peli_intToChar(int nb)
{
	int i;
	int tmp = nb;
	char *result = NULL;

	for(i=0; tmp != 0; i++)
		tmp /= 10;

	result = (char*) malloc(sizeof(char)*(i+1));
	if(result == NULL)
		return NULL;

	sprintf(result, "%d", nb);

	return result;
}

/*
 * -------- peli_initTree --------
 * Description : Allocate and initialise recursively the tree representing the hierarchy of the Workshops
 * Parameters : - The client node (the father in the tree)				client<node*>
 *              - A pointer to a variable representing the IWID to use	absoluteId<int*>
 *              - The current node's depth in the tree					deep<unsigned int>
 *              - The maximum deep in the tree							deepMax<unsigned int>
 *              - The numbre of suppliers of each Workshop				breadth<unsigned int>
 * Return : The initialised node										N<node*>
 * -------------------------------
 */
node *peli_initTree(node *client, unsigned int *absoluteId, unsigned int deep, unsigned int deepMax, unsigned int breadth)
{
	node *N;
	int i;

	if(deep >= deepMax)
		return NULL;

	N = (node*) malloc(sizeof(node));
	N->client = client;
	N->suppliers = (node**) malloc(sizeof(node*)*breadth);
	N->deep = deep;
	N->id = ++(*absoluteId);
	N->pid = -1;
	if(deep+1 >= deepMax)
		N->nbSuppliers = 0;
	else
		N->nbSuppliers = breadth;

	if(client == NULL)
	{
		N->client = (node*) malloc(sizeof(node));
		N->client->suppliers = (node**) malloc(sizeof(node*));
		N->client->deep = deep-1;
		N->client->id = 1;
		N->client->pid = getpid();
		N->client->nbSuppliers = 1;
		N->client->suppliers[0] = N;
	}

	for(i=0; i<breadth; i++)
		N->suppliers[i] = peli_initTree(N, absoluteId, deep+1, deepMax, breadth);

	return N;
}

/*
 * -------- peli_intToChar --------
 * Description : Display the tree in the consol
 * Parameters : the tree's root node	N<node*>
 * Return : 
 * --------------------------------
 */
void peli_displayTree(node *N)
{
	int i;

	if(N == NULL)
		return;
	printf("\n");
	for(i=0; i<N->deep; i++)
		printf("\t");
	printf("-->%d!%ld!%d", N->id, (long) N->pid, N->nbSuppliers);
	if(N->nbSuppliers > 0)
		printf("(%d!%d)", N->suppliers[0]->id, N->suppliers[1]->id);
	printf("\n");
	
	for(i=0; i<N->nbSuppliers; i++)
		peli_displayTree(N->suppliers[i]);
}

/*
 * -------- peli_deleteTree --------
 * Description : Display the tree in the consol
 * Parameters : the tree's root node	N<node*>
 * Return : 
 * ---------------------------------
 */
void peli_deleteTree(node *N)
{
	int i;

	if(N == NULL)
		return;

	for(i=0; i<N->nbSuppliers; i++)
		peli_deleteTree(N->suppliers[i]);

	free(N->suppliers);
	free(N);
}

void peli_createWorkstation(node *N)
{
	int i;
	pid_t pid;
	char **args = NULL;

	if(N == NULL)
		return;

	switch(pid=fork())
	{
		case -1:
			return;
			break;

		case 0:
			args = (char**) malloc(sizeof(char*)*(N->nbSuppliers+4));
			args[0] = (char*) malloc(sizeof(char)*(8));
			strcpy(args[0], "atelier");
			args[1] = peli_intToChar(N->id);
			args[2] = peli_intToChar(N->client->id);
			for(i=0; i<N->nbSuppliers; i++)
				args[i+3] = peli_intToChar(N->suppliers[i]->id);
			args[i+3] = NULL;
			execv("./atelier",args);
			printf("ERRNO\n");
			exit(1);
			break;

		default:
			N->pid = pid;
			break;
	}

	if(N->nbSuppliers > 0)
		for(i=0; i<N->nbSuppliers; i++)
			peli_createWorkstation(N->suppliers[i]);
}
/************************** HMI *********************************************************/
void displayPelikanbanlogo ()
{
	printf("\n\n************************************************************************\n");
	printf("*                                             _______                  *\n");	
	printf("*                                         _.-'       ''...._           *\n");
	printf("*     PeliKanban                        .'        .--.    '.`          *\n");
	printf("*                                      : .--.    :    :     '-.        *\n");
	printf("*       LO41 Project                  : :    :   :    :       :`       *\n");
	printf("*                                     : :  @ :___:  @ : __     '`.     *\n");
	printf("*                                 __..:---''''   `----''  `.   .'      *\n");
	printf("*                         __..--''                   ___j  :   :       *\n");
	printf("*                 __..--''    .--'             __..''      :    `.     *\n");
	printf("*           ..--''                     __..--''        __..'   /``     *\n");
	printf("*         .'                   __..--''        __..--''       /        *\n");
	printf("*        :             __..--''        __..--''               \\        *\n");
	printf("*       :        _.--''        __..--''    :                :`.:       *\n");
	printf("*      :     _.-'      __..--''             :              /           *\n");
	printf("*      :   .'  __..--''                      \\            /            *\n");
	printf("*      \\  :--''                               \\          .'            *\n");
	printf("*       \\ :                                    :         :             *\n");
	printf("*        \\:                                     :        \\             *\n");
	printf("*         '                                     :         \\            *\n");
	printf("************************************************************************\n\n");
	printf("                Welcome and Greatings in PeliKanban\n\n");
}

/*
void HMI_createfactory ()
{	
	unsigned int absolute = 1, depth, suppliers;
	printf("To create a factory, you need to imput some parameters to define the production line :\n");
	printf(" -> The depth of the production line : ");
	scanf("%d",&depth);
	printf("%d\n", depth);
	printf(" -> The number of supplier workshops per workshop : ");
	scanf("%d",&suppliers);
	printf("%d\n", suppliers);
	printf("Thank you ! Initialization in process\n");

	peli_initIPC();
	node *N = peli_initTree(NULL, &absolute, 1, depth, suppliers);
	peli_createWorkstation(N);
	printf("Initialization Completed\n");

}

void HMI ()
{
	int choice;
	displayPelikanbanlogo();

	printf("\n Please select an option :\n");
	printf("1 - Create a factory\n");
	printf("9 - Quit\n");
	printf("Your Choice : ");
	scanf("%d",&choice);
	printf("%d\n", choice);
	while(choice != 1 && choice != 9)
	{
		printf("Wrong input, Please try again\nYour Choice : ");
		scanf("%d",&choice);
		printf("\n");
	}
	switch(choice)
	{
		case 1:
			HMI_createfactory();
			break;
		case 9:  //a redefinir
			break;
}
*/

/************************** HMI *********************************************************/
int main(int argc, char **argv)
{
	int i;
	Message m;
	unsigned int absolute = 1;

	peli_initIPC();
	node *N = peli_initTree(NULL, &absolute, 1, 5, 2);
	peli_createWorkstation(N);
	//HMI ();
	
	sleep(1);
	printf("~~~~\n%d\n~~~~\n", msgid);
	sleep(2);

	m = peli_rcvIPC(0);
	if(m.request == REQ_PING)
		peli_sendIPC(m.from, REQ_REPLY_PING, 0);
	
	peli_deleteTree(N);
	sleep(5);
	peli_destroyIPC();
	return 0;
}

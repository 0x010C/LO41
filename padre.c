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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*
Note
msgget(key, 0750 | IPC_CREAT | IPC_EXCL)
key = ftok(argv[0], 'A')
msgsnd(msgid, &requete, sizeof(trequeteClient) - sizeof(long), 0)
longMSG = msgrcv(msgid, &reponse, sizeof(treponse) - sizeof(long), getpid(), 0)

*/

int msgid;

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

enum req_t {
	REQ_SEND_TICKET,
	REQ_SEND_CONTAINER
};
typedef enum req_t req_t;

typedef struct Message Message;
struct Message
{
	long to;
	long from;
	req_t request;
	long value;
};

void peli_initIPC()
{
	msgid = msgget(ftok("PeliKanban", 42), 0750 | IPC_CREAT | IPC_EXCL);
}

void peli_destroyIPC()
{
	msgctl(msgid, IPC_RMID, NULL);
}

void peli_sendIPC(long to, req_t request, long value)
{
	int i;

	Message toSend;

	toSend.from = 0;
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

req_t peli_getIPC(int flag)
{
	req_t letter;
	msgrcv(msgid, &letter, sizeof(Message) - sizeof(long), 0, flag);
	return letter;
}

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

	for(i=0; i<breadth; i++)
		N->suppliers[i] = peli_initTree(N, absoluteId, deep+1, deepMax, breadth);

	return N;
}

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

int main(int argc, char **argv)
{
	unsigned int absolute = 1;
	node *N = peli_initTree(NULL, &absolute, 0, 4, 2);
	peli_createWorkstation(N);
	peli_displayTree(N);
	peli_deleteTree(N);
	return 0;
}

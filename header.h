#ifndef __MAIN_H__
#define __MAIN_H__


/*
 * ----------------------------
 * INCLUDES
 * ----------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


/*
 * ----------------------------
 * GLOBAL VARIABLES
 * ----------------------------
*/

int msgid;

unsigned int myId;
unsigned int clientId;
unsigned int *suppliersId;


/*
 * ----------------------------
 * ENUMERATIONS
 * ----------------------------
*/

/*
 * enum bool
 * A boolean enumeration
 */
enum bool {
	false,
	true
};
typedef enum bool bool;

/*
 * enum req_t
 * All the possible requests types that the workshops can send to the other one
 */
enum req_t {
	REQ_NULL,
	REQ_SEND_TICKET,
	REQ_SEND_CONTAINER,
	REQ_READY,
	REQ_START,
	REQ_PING,
	REQ_REPLY_PING,
	REQ_SHUTDOWN,
	REQ_CONFIRM_SHUTDOWN,
	REQ_INFORM_NB_IN_CONTAINER
};
typedef enum req_t req_t;


/*
 * ----------------------------
 * STRUCTURES
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
 * HEADERS
 * ----------------------------
*/
void ipc_init(bool create);
void ipc_destroy();
void ipc_send(long to, req_t request, long value);
Message ipc_rcv(int flag);

char *intToChar(int nb);
node *tree_init(node *client, unsigned int *absoluteId, unsigned int deep, unsigned int deepMax, unsigned int breadth);
void tree_display(node *N);
void tree_delete(node *N);

void ws_create(node *N);
void ws_readyStart(unsigned int nbWS);

void hmi_displayPelikanbanlogo();
void hmi_createfactory();
void hmi_menu();


#endif

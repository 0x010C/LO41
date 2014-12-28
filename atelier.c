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
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

enum bool {
	false,
	true
};
typedef enum bool bool;

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

typedef struct Message Message;
struct Message
{
	long to;
	long from;
	req_t request;
	long value;
};

int msgid;
unsigned int myId;
unsigned int clientId;
unsigned int *suppliersId;

void peli_initIPC()
{
	msgid = msgget(ftok("PeliKanban", 42), 0);
}

void peli_sendIPC(long to, req_t request, long value)
{
	int i;

	Message toSend;

	toSend.from = myId;
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

Message peli_rcvIPC(int flag)
{
	Message letter;
	letter.to = -1;
	letter.from = -1;
	letter.request = REQ_NULL;
	letter.value = -1;
	msgrcv(msgid, &letter, sizeof(Message) - sizeof(long), myId, flag);
	return letter;
}

int main(int argc, char **argv)
{
       /**********************************************************************************************************
	*
	*	arguments : 0 : call, 1 : id father, 2 : id child1, 3 : id child2, 4 : id child3
	*	WORKSTATION INITIALIZATION 
	*	STAND BY, READY FOR INSTRUCTION
	*	READ MESSAGE
	*	ACTION
	*	
	**********************************************************************************************************/
	int i, j;
	Message m;
	m.request = -1;
	bool taskWaiting = false;
	int flag;
	unsigned int nbSuppliers;
	unsigned int nbContainerToProduce = 0;
	unsigned int **container;
	unsigned int nbInContainer = rand()%100+1;

	peli_initIPC();

	myId = atoi(argv[1]);
	clientId = atoi(argv[2]);
	suppliersId = (int*) malloc(sizeof(int)*(argc-3));
	nbSuppliers = argc-3;
	container = (unsigned int**) malloc(sizeof(unsigned int*)*(nbSuppliers));
	for(i=0; i<nbSuppliers; i++)
	{
		suppliersId[i] = atoi(argv[i+3]);
		
		container[i] = (unsigned int*) malloc(sizeof(unsigned int)*3);
		container[i][0] = suppliersId[i];
		container[i][1] = 0;
		container[i][2] = 0;
	}

	/*peli_sendIPC(clientId, REQ_INFORM_NB_IN_CONTAINER, nbInContainer);
	if(nbSuppliers>0)printf("[[%d]] send value '%d' to client %d\n", myId, nbInContainer, clientId);
	fflush(stdout);
	for(i=0; i<nbSuppliers; i++)
	{
		do {
			printf("{{%d}} wait\n", myId);
			fflush(stdout);
			m = peli_rcvIPC(0);
			printf("{{%d}} recived\n\tto:%ld\n\tfrom:%ld\n\trequest:%d\n\tvalue:%ld\n", myId, m.to, m.from, m.request, m.value);
			fflush(stdout);
		}while(m.request != REQ_INFORM_NB_IN_CONTAINER);
		printf("==%d== Start looping (%ld)\n", myId, m.from);
		fflush(stdout);
		j=0;
		while(container[j][0] != m.from)
			j++;
		container[j][1] = m.value;
		container[j][2] = m.value;
		printf("==%d== End looping ([%d]%d)\n", myId, j, container[j][0]);
		fflush(stdout);
	}
	if(nbSuppliers>0)printf("##%d## END OF INIT\n", myId);
	fflush(stdout);*/
	
	peli_sendIPC(clientId, REQ_INFORM_NB_IN_CONTAINER, nbInContainer);
	
	
	while(m.request != REQ_SHUTDOWN)
	{
		if(taskWaiting)
			flag = IPC_NOWAIT;
		else
			flag = 0;
		m = peli_rcvIPC(flag);
		switch(m.request)
		{
			case -1:
				break;
			case REQ_PING:
				peli_sendIPC(m.from, REQ_REPLY_PING, m.value);
				break;
			case REQ_SEND_TICKET:
				nbContainerToProduce++;
				break;
			case REQ_SEND_CONTAINER:
				
				break;
			
		}
	}

	peli_sendIPC(1, REQ_CONFIRM_SHUTDOWN, 0);

	return 0;
}

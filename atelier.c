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

enum req_t {
	REQ_SEND_TICKET,
	REQ_SEND_CONTAINER,
	REQ_PING,
	REQ_REPLY_PING,
	REQ_SHUTDOWN,
	REQ_CONFIRM_SHUTDOWN
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
int myId;
int clientId;
int *suppliersId;

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
	letter.request = -1;
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
	int i;
	Message m;
	m.request = -1;
	
	myId = atoi(argv[1]);
	clientId = atoi(argv[2]);
	suppliersId = (int*) malloc(sizeof(int)*(argc-3));
	for(i=0; i<argc-3; i++)
		suppliersId[i] = atoi(argv[i+3]);
	
	peli_initIPC();
	
	while(m.request != REQ_SHUTDOWN)
	{
		m = peli_rcvIPC(0);
		switch(m.request)
		{
			case REQ_PING:
				peli_sendIPC(m.from, REQ_REPLY_PING, m.value);
				break;
		}
	}

	peli_sendIPC(1, REQ_CONFIRM_SHUTDOWN, 0);

	return 0;
}

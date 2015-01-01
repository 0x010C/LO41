#include "header.h"

/*
 * -------- ipc_init --------
 * Description : Create and initialise the IPC object
 * Parameters :
 * Return : nothing but set the global variable msgid
 * ------------------------------
 */
void ipc_init(bool create)
{
	int flag = 0;

	if(create)
		flag = 0750 | IPC_CREAT | IPC_EXCL;

	msgid = msgget(ftok("PeliKanban", 42), flag);

	if(msgid == -1)
	{
		if(create)
		{
			msgid = msgget(ftok("PeliKanban", 42), 0);
			ipc_destroy();
			msgid = msgget(ftok("PeliKanban", 42), flag);
		}
		if(msgid == -1)
		{
			printf("Error : Cannot create an IPC object\n");
			exit(1);
		}
	}
}

/*
 * -------- ipc_destroy --------
 * Description : Destroy the IPC object
 * Parameters :
 * Return :
 * -----------------------------
 */
void ipc_destroy()
{
	msgctl(msgid, IPC_RMID, NULL);
}

/*
 * -------- ipc_send --------
 * Description : Send an IPC message
 * Parameters : - The IWID of the receiver			to<long>
 *              - The type of the request			request<req_t>
 *              - The value to join to the message	value<long>
 * Return :
 * --------------------------
 */
void ipc_send(long to, req_t request, long value)
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

/*
 * -------- ipc_rcv --------
 * Description : Check if there is one message for me, if not wait or pass depending of the value of the flag
 * Parameters : Flag passing to msgrcv, will specify for example if the process wait or not (IPC_NOWAIT) when there is no messages		flag<int>
 * Return : the message recieved, or message null if no message where get																letter<Message>
 * -------------------------
 */
Message ipc_rcv(int flag)
{
	Message letter;
	letter.to = -1;
	letter.from = -1;
	letter.request = REQ_NULL;
	letter.value = -1;
	msgrcv(msgid, &letter, sizeof(Message) - sizeof(long), myId, flag);
	return letter;
}


#include "header.h"

/*
 * -------- ws_create --------
 * Description : Fork the padre to create all the WorkStations following the tree N
 * Parameters : the tree's root node	N<node*>
 * Return : 
 * ---------------------------
 */
void ws_create(node *N)
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
			args[1] = intToChar(N->id);
			args[2] = intToChar(N->client->id);
			for(i=0; i<N->nbSuppliers; i++)
				args[i+3] = intToChar(N->suppliers[i]->id);
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
			ws_create(N->suppliers[i]);
}

/*
 * -------- ws_readyStart --------
 * Description : Wait that all the WorkStations are ready, then send them a message to make them start
 * Parameters : the number of WorkStations	nbWS<unsigned int>
 * Return : 
 * ---------------------------
 */
void ws_readyStart(unsigned int nbWS)
{
	int i = 0;
	Message m;

	while(i<nbWS-1)
	{
		m = ipc_rcv(0);
		if(m.request == REQ_READY)
			i++;
		printf("\t\t\t %d(%ld) get %d from %ld\n", myId, m.to, m.request, m.from);
	}
	printf("\n====================\nAll the ws are Ready, Sending REQ_START signal\n====================\n\n");
	for(i=2;i<=nbWS;i++)
	{
		ipc_send(i, REQ_START, 0);
		printf("\tSend START to %d\n", i);
	}
}

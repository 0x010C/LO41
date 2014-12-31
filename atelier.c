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
#include "header.h"


int main(int argc, char **argv)
{
	/* Declaration */
	int i, j;
	Message m;
	unsigned int nbSuppliers = 0;
	unsigned int nbContainerToProduce = 0;
	unsigned int nbPiecesProduced = 0;
	unsigned int **container;
	unsigned int nbInContainer = 0;
	bool needContainer = false;


	/* IPC Initialisation */
	ipc_init(false);
	/* IWIDs initialisations based on the parameters */
	myId = atoi(argv[1]);
	clientId = atoi(argv[2]);
	suppliersId = (int*) malloc(sizeof(int)*(argc-3));
	nbSuppliers = argc-3;
	container = (unsigned int**) malloc(sizeof(unsigned int*)*(nbSuppliers));
	srand(myId+time(NULL));

	for(i=0; i<nbSuppliers; i++)
	{
		suppliersId[i] = atoi(argv[i+3]);
		container[i] = (unsigned int*) malloc(sizeof(unsigned int)*3);
		container[i][0] = 0;
		container[i][1] = 0;
		container[i][2] = 0;
	}


	/* Reception from the suppliers of the number of pieces per container */
	for(i=0; i<nbSuppliers; i++)
	{
		m = ipc_rcv(0);
		if(m.request == REQ_INFORM_NB_IN_CONTAINER)
		{
			j=0;
			while(j < nbSuppliers)
			{
				if(suppliersId[j] == m.from)
				{
					container[j][0] = m.value;
					container[j][1] = m.value;
					break;
				}
				j++;
			}
		}
	}
	/* Computation of the number of pieces per container, based on the current IWID */
	if(myId == 2)
		nbInContainer = 1;
	else
		nbInContainer = myId*7+rand()%7;
	/* Transmission to the client of the number of pieces per container */
	ipc_send(clientId, REQ_INFORM_NB_IN_CONTAINER, nbInContainer);


	/* Send READY Message */
	ipc_send(1, REQ_READY, 0);
	/* And wait for the START signal */
	do
	{
		m = ipc_rcv(0);
	}while(m.request != REQ_START);
	printf("# %d start working\n", myId);


	/*
	 * =========
	 * Main Loop
	 * =========
	 */
	do
	{
		/* Receiving the messages (wait if there is nothing to do, dont in other cases) */
		if(nbContainerToProduce > 0 && !needContainer)
			m = ipc_rcv(IPC_NOWAIT);
		else
			m = ipc_rcv(0);

		/* Analyse the message */
		switch(m.request)
		{
			case REQ_NULL:
				break;
			case REQ_SEND_TICKET:
				nbContainerToProduce += m.value;
				break;
			case REQ_SEND_CONTAINER:
				while(i < nbSuppliers)
				{
					if(suppliersId[i] == m.from)
					{
						if(container[i][0] == 0)
							container[i][0] = m.value;
						else if(container[i][1] == 0)
							container[i][1] = m.value;
						else
							container[i][2] = m.value;
						break;
					}
					i++;
				}
				break;
			case REQ_PING:
				ipc_send(m.from, REQ_REPLY_PING, m.value);
				break;
			case REQ_SHUTDOWN:
				break;
			default:
				break;
		}
		
		needContainer = false;
		
		/* If there is something to produce */
		if(nbContainerToProduce > 0)
		{
			/* Check if there is no leak of pieces */
			for(i=0; i<nbSuppliers; i++)
			{
				if(container[i][0] == 0)
				{
					needContainer = true;
					break;
				}
			}
			/* If not, we can produce */
			if(!needContainer)
			{
				/* We take one piece from each suppliers' container */
				for(i=0; i<nbSuppliers; i++)
				{
					container[i][0]--;
					/* If the container 0 became empty, we ask our supplier a new one and we transfert the content from the other containers */
					if(container[i][0] == 0)
					{
						ipc_send(suppliersId[i], REQ_SEND_TICKET, 1); /*TODO: Send it when we open the container at the beginning*/

						container[i][0] = container[i][1];
						container[i][1] = container[i][2];
						container[i][2] = 0;
					}
				}
				/* ... Factory work ... */
				/* Our piece is built */
				nbPiecesProduced++;
				/* ... Factory work ... */
				
				/* If a container is filled */
				if(nbPiecesProduced == nbInContainer)
				{
					/* We send the container to our client */
					ipc_send(clientId, REQ_SEND_CONTAINER, nbPiecesProduced);
					/* Actualisation of the factory's datas */
					nbPiecesProduced = 0;
					nbContainerToProduce--;
				}
			}
		}
	}while(m.request != REQ_SHUTDOWN);

	ipc_send(1, REQ_CONFIRM_SHUTDOWN, 0);

	return 0;
}

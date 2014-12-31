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
	int i, j;
	Message m;
	m.request = -1;
	bool taskWaiting = false;
	int flag;
	unsigned int nbSuppliers;
	unsigned int nbContainerToProduce = 0;
	unsigned int **container;
	unsigned int nbInContainer;

	ipc_init(false);

	myId = atoi(argv[1]);
	clientId = atoi(argv[2]);
	suppliersId = (int*) malloc(sizeof(int)*(argc-3));
	nbSuppliers = argc-3;
	container = (unsigned int**) malloc(sizeof(unsigned int*)*(nbSuppliers));
	srand(myId+time(NULL));

	for(i=0; i<nbSuppliers; i++)
	{
		suppliersId[i] = atoi(argv[i+3]);
		container[i] = (unsigned int*) malloc(sizeof(unsigned int)*2);
		container[i][0] = 0;
		container[i][1] = 0;
	}

	for(i=0; i<nbSuppliers; i++)
	{
		m = ipc_rcv(0);
		if(m.request == REQ_INFORM_NB_IN_CONTAINER)
		{
			//printf("> %d recieved info from %ld (%ld)\n", myId, m.from, m.value);
			j=0;
			while(j < nbSuppliers)
			{
				//printf("\t\t[%d : %d <> %ld]\n", myId, suppliersId[j], m.from);
				if(suppliersId[j] == m.from)
				{
					container[j][0] = m.value;
					container[j][1] = m.value;
					//printf("\t{%d saved value %ld in suppliers %d}\n", myId, m.value,suppliersId[j]);
					break;
				}
				j++;
			}
		}
	}
	if(myId == 2)
		nbInContainer = 1;
	else
		nbInContainer = myId*7+rand()%7;
	ipc_send(clientId, REQ_INFORM_NB_IN_CONTAINER, nbInContainer);
	ipc_send(1, REQ_READY, 0);
	printf("# %d is ready\n", myId);
	do
	{
		m = ipc_rcv(0);
		printf("# %d get %d from %ld\n", myId, m.request, m.from);
	}while(m.request != REQ_START);
	printf("# %d start working\n", myId);

	return 0;
}

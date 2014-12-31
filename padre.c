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
	int i = 0;
	Message m;
	unsigned int nbWS = 1;
	
	myId = 1;

	ipc_init(true);
	node *N = tree_init(NULL, &nbWS, 1, 5, 2);
	printf("~~~~~~~~~~\n%d\n~~~~~~~~~~\n", nbWS);
	sleep(1);
	ws_create(N);

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
	
	sleep(7);
	tree_delete(N);
	ipc_destroy();
	return 0;
}

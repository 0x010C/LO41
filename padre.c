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
	unsigned int nbWS = 1;
	int i;
	Message m;

	myId = 1;

	ipc_init(true);
	node *N = tree_init(NULL, &nbWS, 1, 6, 5);
	sleep(1);
	ws_create(N);

	ws_readyStart(nbWS);

	ipc_send(2, REQ_SEND_TICKET, 100000);
	for(i=0; i<100000; i++)
		do{
			m = ipc_rcv(0);
		}
		while(m.request != REQ_SEND_CONTAINER);

	printf("\n==========\n   END\n==========\n\n");

	ws_shutdown(nbWS);
	printf("\n==========\n   END\n==========\n\n");
	
	tree_delete(N);
	ipc_destroy();
	return 0;
}

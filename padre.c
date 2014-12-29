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
	int i;
	Message m;
	unsigned int absolute = 1;
	
	myId = 1;

	ipc_init(true);
	node *N = tree_init(NULL, &absolute, 1, 5, 2);
	ws_create(N);
	//HMI ();
	
	sleep(1);
	printf("~~~~\n%d\n~~~~\n", msgid);
	sleep(2);

	m = ipc_rcv(0);
	if(m.request == REQ_PING)
		ipc_send(m.from, REQ_REPLY_PING, 0);
	
	tree_delete(N);
	sleep(7);
	ipc_destroy();
	return 0;
}

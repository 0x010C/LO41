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
	REQ_SEND_CONTAINER
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
	if(argc == 5)
		printf("<%s>\n    Client : %s\n    Fournisseurs : {%s;%s}\n", argv[1], argv[2], argv[3], argv[4]);
	else if(argc == 3)
		printf("<%s>\n    Client : %s\n    Fournisseurs : {}\n", argv[1], argv[2]);
	else
		printf("ARGC\n");
	
	return 0;
}

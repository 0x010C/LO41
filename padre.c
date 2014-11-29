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
#include <sys/types.h>
#include <unistd.h>

typedef struct node node;
struct node {
	node *client;
	node **suppliers;
	unsigned int deep;
	unsigned int nbSuppliers;
	unsigned int id;
	pid_t pid;
};

char *peli_intToChar(int nb)
{
	int i;
	int tmp = nb;
	char *result = NULL;

	for(i=0; tmp != 0; i++)
		tmp /= 10;

	result = (char*) malloc(sizeof(char)*(i+1));
	if(result == NULL)
		return NULL;

	sprintf(result, "%d", nb);

	return result;
}

node *peli_initTree(node *client, unsigned int *absoluteId, unsigned int deep, unsigned int deepMax, unsigned int breadth)
{
	node *N;
	int i;

	if(deep >= deepMax)
		return NULL;

	N = (node*) malloc(sizeof(node));
	N->client = client;
	N->suppliers = (node**) malloc(sizeof(node*)*breadth);
	N->nbSuppliers = breadth;
	N->deep = deep;
	N->id = ++(*absoluteId);
	N->pid = -1;

	for(i=0; i<breadth; i++)
		N->suppliers[i] = peli_initTree(N, absoluteId, deep+1, deepMax, breadth);

	return N;
}

void peli_displayTree(node *N)
{
	int i;

	if(N == NULL)
		return;

	for(i=0; i<N->deep; i++)
		printf("\t");
	printf("-%d\n", N->id);
	
	for(i=0; i<N->nbSuppliers; i++)
		peli_displayTree(N->suppliers[i]);
}

int peli_createWorkstation(unsigned id, unsigned int cliId, unsigned int deep, unsigned int deepMax, unsigned int breadth)
{
	/*int i;
	char **args = NULL;*/

	/* If we have reached the maximum deep, we stop creating Workstations */	
	/*if(deep >= deepMax)
		return 0;*/
	
	/* Else, we create first the n-Suppliers */
	/*for(i=1; i<=breadth; i++)
	{
		if(!peli_createWorkstation(id+i, id, deep+1, deepMax, breadth))
			return 1;
	}*/

	/* And then, we create the current Workstation */
	/*switch(fork())
	{
		case -1:
			return 1;
			break;
	
		case 0:
			
			charDeep = peli_intToChar();
			if(charDeep == NULL)
				return 2;
			execl("./atelier", "atelier", charDeep);
			return 3;
			break;
	
		default:
			break;
	}*/
	return 0;
}

int main(int argc, char **argv)
{
	unsigned int absolute = 0;
	node *N = peli_initTree(NULL, &absolute, 0, 4, 2);
	peli_displayTree(N);
	return 0;
}

#include "header.h"

/*
 * -------- intToChar --------
 * Description : Convert an integer into a string
 * Parameters : the integer to convert			nb<int>
 * Return : the dynamicaly allocated string		result<char*>
 * ---------------------------
 */
char *intToChar(int nb)
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

/*
 * -------- tree_init --------
 * Description : Allocate and initialise recursively the tree representing the hierarchy of the Workshops
 * Parameters : - The client node (the father in the tree)				client<node*>
 *              - A pointer to a variable representing the IWID to use	absoluteId<int*>
 *              - The current node's depth in the tree					deep<unsigned int>
 *              - The maximum deep in the tree							deepMax<unsigned int>
 *              - The numbre of suppliers of each Workshop				breadth<unsigned int>
 * Return : The initialised node										N<node*>
 * ----------------------------
 */
node *tree_init(node *client, unsigned int *absoluteId, unsigned int deep, unsigned int deepMax, unsigned int breadth)
{
	node *N;
	int i;

	if(deep >= deepMax)
		return NULL;

	N = (node*) malloc(sizeof(node));
	N->client = client;
	N->suppliers = (node**) malloc(sizeof(node*)*breadth);
	N->deep = deep;
	N->id = ++(*absoluteId);
	N->pid = -1;
	if(deep+1 >= deepMax)
		N->nbSuppliers = 0;
	else
		N->nbSuppliers = breadth;

	if(client == NULL)
	{
		N->client = (node*) malloc(sizeof(node));
		N->client->suppliers = (node**) malloc(sizeof(node*));
		N->client->deep = deep-1;
		N->client->id = 1;
		N->client->pid = getpid();
		N->client->nbSuppliers = 1;
		N->client->suppliers[0] = N;
	}

	for(i=0; i<breadth; i++)
		N->suppliers[i] = tree_init(N, absoluteId, deep+1, deepMax, breadth);

	return N;
}

/*
 * -------- tree_display --------
 * Description : Display the tree in the consol
 * Parameters : the tree's root node	N<node*>
 * Return : 
 * ------------------------------
 */
void tree_display(node *N)
{
	int i;

	if(N == NULL)
		return;
	printf("\n");
	for(i=0; i<N->deep; i++)
		printf("\t");
	printf("-->%d!%ld!%d", N->id, (long) N->pid, N->nbSuppliers);
	if(N->nbSuppliers > 0)
		printf("(%d!%d)", N->suppliers[0]->id, N->suppliers[1]->id);
	printf("\n");
	
	for(i=0; i<N->nbSuppliers; i++)
		tree_display(N->suppliers[i]);
}

/*
 * -------- tree_delete --------
 * Description : Display the tree in the consol
 * Parameters : the tree's root node	N<node*>
 * Return : 
 * -----------------------------
 */
void tree_delete(node *N)
{
	int i;

	if(N == NULL)
		return;

	for(i=0; i<N->nbSuppliers; i++)
		tree_delete(N->suppliers[i]);

	free(N->suppliers);
	free(N);
}

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

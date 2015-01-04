#include "header.h"

void hmi_displayPelikanbanlogo()
{
	printf("\n\n************************************************************************\n");
	printf("*                                             _______                  *\n");	
	printf("*                                         _.-'       ''...._           *\n");
	printf("*     PeliKanban                        .'        .--.    '.`          *\n");
	printf("*                                      : .--.    :    :     '-.        *\n");
	printf("*       LO41 Project                  : :    :   :    :       :`       *\n");
	printf("*                                     : :  @ :___:  @ : __     '`.     *\n");
	printf("*                                 __..:---''''   `----''  `.   .'      *\n");
	printf("*                         __..--''                   ___j  :   :       *\n");
	printf("*                 __..--''    .--'             __..''      :    `.     *\n");
	printf("*           ..--''                     __..--''        __..'   /``     *\n");
	printf("*         .'                   __..--''        __..--''       /        *\n");
	printf("*        :             __..--''        __..--''               \\        *\n");
	printf("*       :        _.--''        __..--''    :                :`.:       *\n");
	printf("*      :     _.-'      __..--''             :              /           *\n");
	printf("*      :   .'  __..--''                      \\            /            *\n");
	printf("*      \\  :--''                               \\          .'            *\n");
	printf("*       \\ :                                    :         :             *\n");
	printf("*        \\:                                     :        \\             *\n");
	printf("*         '                                     :         \\            *\n");
	printf("************************************************************************\n\n");
	printf("                Welcome and Greatings in PeliKanban\n\n");
}


unsigned int hmi_createfactory(unsigned int nbWS, node *N)
{	
	unsigned int absolute = 1, depth, suppliers;
	printf("To create a factory, you need to imput some parameters to define the production line :\n");
	printf(" -> The depth of the production line : ");
	scanf("%d",&depth);
	printf("%d\n", depth);
	printf(" -> The number of supplier workshops per workshop : ");
	scanf("%d",&suppliers);
	printf("%d\n", suppliers);
	printf("Thank you ! Initialization in process\n");

	N = tree_init(NULL, &absolute, 1, depth, suppliers);
	sleep(1);
	ws_create(N);

	ws_readyStart(nbWS);
	printf("Initialization Completed\n");
	return nbWS;
}


void hmi_launchprod(long nb)
{
	Message m;
	int i;
	
	ipc_send(2, REQ_SEND_TICKET, nb);
	for(i=0; i<nb; i++)
		do{
			m = ipc_rcv(0);
		}
		while(m.request != REQ_SEND_CONTAINER);
}


void hmi_shutdown(unsigned int nbWS, node *N)
{
	ws_shutdown(nbWS);
	tree_delete(N);
}
void hmi_menu()
{
	int choice;
	bool factory = false;
	long things;
	unsigned int nbWS = 1;
	node *N;
	hmi_displayPelikanbanlogo();
	
	do
	{
		printf("\n Please select an option :\n");
		
		if (factory == false)
		{
			printf("1 - Create a factory\n");
			printf("9 - Quit\n");
			printf("Your Choice : ");
			scanf("%d",&choice);
			//printf("%d\n", choice);
			while(choice != 1 && choice != 9)
			{
				printf("Wrong input, Please try again\nYour Choice : ");
				
				printf("\n");
			}
			switch(choice)
			{
				case 1:
					nbWS = hmi_createfactory(nbWS, N);
					factory = true;
					printf("Factory Created\n\n");
					break;
				case 9:  //a redefinir
					break;
			}
		} else {
			printf("1 - Launch the production\n");
			printf("2 - Display the tree of the production line\n");
			printf("3 - Delete the production line\n");
			printf("9 - Quit\n");

			printf("Your Choice : ");
			scanf("%d",&choice);
			printf("\n");
			while(choice != 1 && choice != 2 && choice != 3 && choice != 9)
			{
				printf("Wrong input, Please try again\nYour Choice : ");
				scanf("%d",&choice);
				printf("\n");
			}
			switch(choice)
			{
				case 1:
					printf("How many things do you want to produce ? >");
					scanf("%ld",&things);
					printf("%ld");
					hmi_launchprod(things);
					printf("Production finished\n\n");
					break;
				case 2:
					printf("Display of the factory\n\n");
					tree_display(N);
					break;
				case 3:
					printf("Deleting of the production line un progress\n");
					hmi_shutdown(nbWS, N);
					factory = false;
					printf("Factory Deleted\n\n");
					break;

				case 9: 
					printf(" Oh come on ! Stay a litle bit more with us ! We have cookies !\n");
					break;
			}
		}

	}
	while(choice != 9);
}

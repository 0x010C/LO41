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


void hmi_createfactory()
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

	ipc_init(0);
	node *N = tree_init(NULL, &absolute, 1, depth, suppliers);
	ws_create(N);
	printf("Initialization Completed\n");

}

void hmi_menu()
{
	int choice;
	hmi_displayPelikanbanlogo();
	
	do
	{
		printf("\n Please select an option :\n");
		//what function tell you there is a factory ?
		if (0)
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
					hmi_createfactory();
					printf("Factory Created\n\n");
					break;
				case 9:  //a redefinir
					break;
			}
		} else {
			printf("1 - Launch the production\n");
			printf("2 - Display the tree of the production line\n");
			printf("3 - Something funny to do ?\n");
			printf("9 - Quit\n");

			printf("Your Choice : ");
			scanf("%d",&choice);
			//printf("%d\n", choice);
			while(choice != 1 && choice != 2 && choice != 3 && choice != 9)
			{
				printf("Wrong input, Please try again\nYour Choice : ");
				scanf("%d",&choice);
				//printf("\n");
			}
			switch(choice)
			{
				case 1:
					printf("How many things do you want to produce ?")
					hmi_launchprod(nbprod)
					printf("Factory Created\n\n");
					break;
				case 9: 
					printf(" Oh come on ! Stay a litle bit more with us ! We have cookies !\n");
					break;
			}
		}

	}
	while(choice != 9);
}

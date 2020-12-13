//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : Debug.c
//* Object              : Debug menu
//* Creation            : JPP   16/May/2004
//*----------------------------------------------------------------------------

#include <stdio.h>

// Include Standard files
#include "Board.h"
#include "Flash.h"

/*---------------------------- Global Variable ------------------------------*/
char message[30];

//*--------------------------1--------------------------------------------------
//* \fn    AT91F_DBGU_Printk
//* \brief This function is used to send a string through the DBGU channel (Very low level debugging)
//*----------------------------------------------------------------------------
void AT91F_DBGU_Printk(	char *buffer)
{
    while(*buffer != '\0') {
	while (!AT91F_US_TxReady((AT91PS_USART)AT91C_BASE_DBGU));
	AT91F_US_PutChar((AT91PS_USART)AT91C_BASE_DBGU, *buffer++);
    }
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Get
//* \brief Get a Char to USART
//*----------------------------------------------------------------------------
 int AT91F_US_Get( char *val)
{
    if ((AT91F_US_RxReady((AT91PS_USART)AT91C_BASE_DBGU)) == 0) return (false);
    else
    {
	*val= AT91F_US_GetChar((AT91PS_USART)AT91C_BASE_DBGU);
        return (true);
    }
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_scanf
//* \brief Get a string to USART manage Blackspace and echo
//*----------------------------------------------------------------------------
void AT91F_DBGU_scanf(char * type,unsigned int * val)
{//* Begin
    unsigned int read = 0;
    char buff[10];
    unsigned int nb_read =0;

    while( (read != 0x0D) & (nb_read != sizeof(buff)) ) {
        //* wait the USART Ready for reception
	 while((AT91C_BASE_DBGU->DBGU_CSR  & AT91C_US_RXRDY) == 0 ) ;
        //* Get a char
	read = AT91C_BASE_DBGU->DBGU_RHR ;
        buff[nb_read]= (char)read;
        //* Manage Blackspace
        while((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY) ==0)  {}
        if ((char)read == 0x08) {
            if ( nb_read != 0 ) {
              nb_read--;
              AT91C_BASE_DBGU->DBGU_THR = read;
            }
        }
        else {
          //* echo
          AT91C_BASE_DBGU->DBGU_THR = read;
          nb_read++;
        }
    }
    //* scan the value
    sscanf(buff,type,val);
}//* End

//*----------------------------------------------------------------------------
//* \fn   AT91F_DBGU_Flash
//* \brief print address and size
//*----------------------------------------------------------------------------
void AT91F_DBGU_Flash(unsigned int add, unsigned int size)
{
	sprintf( message,"Start 0x%X size 0x%X (%d)\n\r" ,add, size, size);
	AT91F_DBGU_Printk(message);
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Print_Memory
//* \brief Print a memory area add: xx xx xx xx
//*----------------------------------------------------------------------------
void AT91F_Print_Memory(unsigned int add, unsigned int size)
{
   unsigned int i;
   unsigned int * start_add = (unsigned int *) add;
    AT91F_DBGU_Flash(add,size);
    for (i=0 ; i < size ; i+=4)
    {
       sprintf( message,"At 0x%X: 0x%X 0x%X 0x%X 0x%X\n\r",&start_add[i], start_add[i], start_add[i+1], start_add[i+2], start_add[i+3] );
       AT91F_DBGU_Printk(message);
    }
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock_info
//* \brief Print Flash NVM bit
//*----------------------------------------------------------------------------
void AT91F_Flash_Lock_info(void)
{
	sprintf(message,"MC_FSR: 0x%X\n\r",AT91C_BASE_MC->MC_FSR);
	AT91F_DBGU_Printk(message);
	if (AT91F_Flash_Lock_Status()!=0)    AT91F_DBGU_Printk("Lock Bits is set\n\r");
	if (AT91F_NVM_Status() !=0)          AT91F_DBGU_Printk("MVM Bits is set\n\r");
	if (AT91F_SET_Security_Status() !=0) AT91F_DBGU_Printk("Security Bit is set\n\r");
}

//*----------------------------------------------------------------------------
//* \fn    menu
//* \brief Manage the debug menu
//*----------------------------------------------------------------------------
void menu(void)
{
	unsigned int * start_add;
	int i,size,page;
	char val;
	unsigned int buff[FLASH_PAGE_SIZE_LONG*10];
    //* Data initialization
	size = FLASH_PAGE_SIZE_BYTE;
	page = 0;
	start_add = ( unsigned int *) AT91C_IFLASH;
        AT91F_DBGU_Flash (( unsigned int)start_add,size);
	for (i=0; i < FLASH_PAGE_SIZE_LONG*4; i++) { buff[i] = 0xAABBCC00+i;}
    //* Init the flash access
        AT91F_Flash_Init();

	AT91F_DBGU_Printk("1) Flash\n\r2) dump Flash\n\r3) write\n\r4) write page\n\r5) Erase\n\r6) set NVM\n\r7) ClearNVM\n\r8) Set Lock\n\r9) Clear lock\n\rA) Unlock All\n\rB) Lock All\n\r");

    //* Loop menu
	while (1)
	{
        if (AT91F_US_Get(&val))
        switch (val) {

        case '1': //* info
             sprintf( message,"Page:%d\n\r",page);
             AT91F_DBGU_Printk(message);
             AT91F_Flash_Lock_info();
             AT91F_DBGU_Flash((unsigned int)start_add,size);
             AT91F_Print_Memory((unsigned int)start_add,FLASH_PAGE_SIZE_LONG*4);
             if (AT91F_Flash_Check_Erase((unsigned int*)AT91C_IFLASH,AT91C_IFLASH_SIZE/4) )  AT91F_DBGU_Printk("Flash erased\n\r");
             else  AT91F_DBGU_Printk("Flash NOT erased !\n\r");
        break;

        case '2': // dump flash
             AT91F_Print_Memory((unsigned int)start_add,AT91C_IFLASH_SIZE/4);
        break;

        case '3'://* write one page
             AT91F_DBGU_Printk("Page ?:");
             AT91F_DBGU_scanf("%d",(unsigned int*)&page );
             start_add = ( unsigned int *) AT91C_IFLASH + ( page * FLASH_PAGE_SIZE_LONG);
             AT91F_DBGU_Flash((unsigned int)start_add,FLASH_PAGE_SIZE_LONG);
             if (AT91F_Flash_Write((unsigned int)start_add ,FLASH_PAGE_SIZE_BYTE ,(unsigned int*) &buff) )  AT91F_DBGU_Printk("Flash write\n\r");
             else  AT91F_DBGU_Printk("Flash NOT write !\n\r");
             AT91F_Print_Memory((unsigned int)start_add,FLASH_PAGE_SIZE_BYTE/4);
        break;
        case '4'://* write some page
             AT91F_DBGU_Printk("Address (Hex)");
             AT91F_DBGU_scanf("%X",(unsigned int*)&size );
             start_add = (unsigned int *)size;
             AT91F_DBGU_Printk("\n\rSize in byte (int)");
             AT91F_DBGU_scanf("%d",(unsigned int*)&size );

             AT91F_DBGU_Flash((unsigned int)start_add,size);
             if (AT91F_Flash_Write_all((unsigned int)start_add ,size ,(unsigned int*) &buff) )  AT91F_DBGU_Printk("Flash write\n\r");
             else  AT91F_DBGU_Printk("Flash NOT write !\n\r");
             AT91F_Print_Memory((unsigned int)start_add,(size/4));
        break;

        case '5'://* Erase all
              if (AT91F_Flash_Erase_All()) AT91F_DBGU_Printk("Flash cmd OK\n\r");;
              if (AT91F_Flash_Check_Erase((unsigned int*)AT91C_IFLASH,AT91C_IFLASH_SIZE) )  AT91F_DBGU_Printk("Flash erased\n\r");
              else  AT91F_DBGU_Printk("Flash NOT erased !\n\r");
        break;

        case '6'://* Set NVM
              sprintf( message,"SET MNV 1 0x%X\n\r",AT91F_NVM_Set (1));
              AT91F_DBGU_Printk(message);
        break;

        case '7'://* Clear NVM
              sprintf( message,"SET MNV 1 0x%X\n\r",AT91F_NVM_Clear (1));
              AT91F_DBGU_Printk(message);
        break;

        case '8'://* Set Lock bit
             AT91F_DBGU_Printk("Page ?:");
             AT91F_DBGU_scanf("%d",(unsigned int*)&page );
             sprintf( message,"SET lock %d 0x%X\n\r",page,AT91F_Flash_Lock (page));
             AT91F_DBGU_Printk(message);
        break;

        case '9'://* Clear Lock bit
             AT91F_DBGU_Printk("Page ?:");
             AT91F_DBGU_scanf("%d",(unsigned int*)&page );
             sprintf( message,"CLEAR lock %d 0x%X\n\r",page,AT91F_Flash_Unlock (page));
             AT91F_DBGU_Printk(message);
        break;

        case 'A'://* Clear Lock bit
             for (i=0;i<FLASH_PAGE_NB;i+=FLASH_PAGE_LOCK)
             {
                 sprintf(message,"Clear page %d 0x%X\n\r",i,AT91F_Flash_Unlock (i));
                 AT91F_DBGU_Printk(message);
             }
        break;

        case 'B'://* Set Lock bit
              for (i=0;i<FLASH_PAGE_NB;i+=FLASH_PAGE_LOCK)
              {
                   sprintf(message,"SET page %d 0x%X\n\r",i,AT91F_Flash_Lock (i));
                   AT91F_DBGU_Printk(message);
              }
        break;

                default:
                break;
		}// End Switch
	}// End while

}






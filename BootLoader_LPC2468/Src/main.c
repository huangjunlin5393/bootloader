//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FLASH.h"
//#include "AT91SAM7SE32.h"
#include "Dbgu.h"
#include "PIO.h"
//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------
#define arm_code1 				0x80C00000
#define arm_code2 				0x80C80000
#define arm_code_init 			0x80D00000
#define ARM_CODE_SEL			0x10020000
#define CODE_RUN_SDRAM_ADD	0xA0000000




#define INCHIP_FLASH 			0x101000
#define OUTCHIP_COPY 			0x10010000
#define ARM_CODE_Backup 		0x10020800
#define arm_code_base1 		0xffff
#define chip_sel 				0x0a0a



//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------
extern void JUMPTO(unsigned int jumpaddr);
//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//int (*jump)(void);

extern void BootProgram(unsigned int EntryAdd);

void LoadImage(unsigned int addr, int len)
{
    unsigned int *from,*to;
    int i =0;    
    from = 	(unsigned int *)addr;
    to   = 	(unsigned int *)CODE_RUN_SDRAM_ADD;
    AT91F_DBGU_Printk("\r\n start to copy image to ram");
	
    while( i++ < len)
    {
    	*to = *from;
    	from++;
    	to++;
    }
    BootProgram(CODE_RUN_SDRAM_ADD);
    return;
}

//------------------------------------------------------------------------------
/// Main function
//------------------------------------------------------------------------------
int main(void)
{
	unsigned int i=0;
	unsigned int j=0;
	volatile unsigned int length=0;
	unsigned int addr_temp=arm_code1;
	unsigned int * start_add;
	Pin WDT=PIN_WDT;//watch dog        

	unsigned int* pFlash=(unsigned int*)(arm_code1);

	PIO_CfgOutput(&WDT);
	PIO_Clear(&WDT);
	AT91F_DBGU_CfgPIO();
	PIO_Set(&WDT);
	AT91F_US_Configure ((AT91PS_USART) AT91C_BASE_DBGU, 47923200,AT91C_US_ASYNC_MODE , 115200,0);                                   // Timeguard to be programmed
	((AT91PS_USART)AT91C_BASE_DBGU)->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOA);
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOB);
	PIO_Clear(&WDT);
	AT91F_DBGU_Printk("haha, B is working \n");
	PIO_Set(&WDT);

	if (*(unsigned short*)(ARM_CODE_Backup)==chip_sel)
		addr_temp=arm_code_init;
	else
	{
		if (*(unsigned short*)(0x10020100)==arm_code_base1)
		{
			if (Check_Write_Important(arm_code1,false,INCHIP_FLASH)==right)
				addr_temp=arm_code1;
			else
			{
				if (Check_Write_Important(arm_code2,false,INCHIP_FLASH)==right)
					addr_temp=arm_code2;
				else
					addr_temp=arm_code_init;				
			}				
		}			
		else
		{
			if (Check_Write_Important(arm_code2,false,INCHIP_FLASH)==right)
				addr_temp=arm_code2;
			else
			{
				if (Check_Write_Important(arm_code1,false,INCHIP_FLASH)==right)
					addr_temp=arm_code1;
				else
					addr_temp=arm_code_init;				
			}	
		}	
	}
	PIO_Clear(&WDT);

	length=*(unsigned int *)(addr_temp+0x10);
	if (length>0x3800)
	{
		AT91F_DBGU_Printk("too long&change to init code \n");
		length=0x3800;
		addr_temp=arm_code_init;				
	}
	PIO_Set(&WDT);
	
	addr_temp=addr_temp+0x200;//Ìø¹ýÍ·²¿
	LoadImage(addr_temp, length);
	
#if 0
	if (length%0x40==0)
		length=length/0x40;
	else
		length=(length/0x40)+1;
		
	start_add = ( unsigned int *) INCHIP_FLASH;

	while(1)
	{
		PIO_Clear(&WDT);
		addr_temp=addr_temp+0x200;
		for (i=0;i<length;i++)
		{
			PIO_Set(&WDT);
		    start_add=( unsigned int *) (INCHIP_FLASH+i*0x80);
			pFlash=(unsigned int *)(addr_temp+i*0x80);
		 	AT91F_Flash_Write((unsigned int)start_add ,FLASH_PAGE_SIZE_BYTE,pFlash);
			for (j=0;j<0x100;j++)
			{}
			PIO_Clear(&WDT);
		}

		addr_temp=addr_temp-0x200;
		if (Check_Write_Important(addr_temp, true, INCHIP_FLASH)==right)
		{
			PIO_Set(&WDT);

			BootProgram(INCHIP_FLASH);
		
		}

	}
#endif
}

/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the protocols
 * Author: Zhaoym
 * Created: 2008/03/27
 * Modified：Liulh 2009/11/08
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Dbgu.h"
#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

int AT91F_US_PutChar (AT91PS_USART pUSART,	int character )
{
    pUSART->US_THR = (character & 0x1FF);
}

int AT91F_US_TxReady (AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR & AT91C_US_TXRDY);
}


void AT91F_US_Configure (
	AT91PS_USART pUSART,     // \arg pointer to a USART controller
	unsigned int mainClock,  // \arg peripheral clock
	unsigned int mode ,      // \arg mode Register to be programmed
	unsigned int baudRate ,  // \arg baudrate to be programmed
	unsigned int timeguard ) // \arg timeguard to be programmed
{
    //* Disable interrupts
    pUSART->US_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
    pUSART->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS ;

	//* Define the baud rate divisor register
	AT91F_US_SetBaudrate(pUSART, mainClock, baudRate);

	//* Write the Timeguard Register
	AT91F_US_SetTimeguard(pUSART, timeguard);

    //* Clear Transmit and Receive Counters
    AT91F_PDC_Open((AT91PS_PDC) &(pUSART->US_RPR));

    //* Define the USART mode
    pUSART->US_MR = mode  ;

}
void AT91F_US_SetBaudrate (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int mainClock, // \arg peripheral clock
	unsigned int speed)     // \arg UART baudrate
{
	//* Define the baud rate divisor register
	pUSART->US_BRGR = AT91F_US_Baudrate(mainClock, speed);
}
void AT91F_US_SetTimeguard (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int timeguard) // \arg timeguard value
{
	//* Write the Timeguard Register
	pUSART->US_TTGR = timeguard ;
}

unsigned int AT91F_US_Baudrate (
	const unsigned int main_clock, // \arg peripheral clock
	const unsigned int baud_rate)  // \arg UART baudrate
{
	unsigned int baud_value = ((main_clock*10)/(baud_rate * 16));
	if ((baud_value % 10) >= 5)
		baud_value = (baud_value / 10) + 1;
	else
		baud_value /= 10;
	return baud_value;
}
void AT91F_DBGU_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA9_DRXD    ) |
		((unsigned int) AT91C_PA10_DTXD    ), // Peripheral A
		0); // Peripheral B
}
void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable)  // \arg PERIPH B to enable

{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); // Set in Periph mode
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Open
//* \brief Open PDC: disable TX and RX reset transfer descriptors, re-enable RX and TX
//*----------------------------------------------------------------------------
void AT91F_PDC_Open (
	AT91PS_PDC pPDC)       // \arg pointer to a PDC controller
{
    //* Disable the RX and TX PDC transfer requests
	pPDC->PDC_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

    //* Reset all Counter register Next buffer first
	AT91F_PDC_SetNextTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetNextRx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetRx(pPDC, (char *) 0, 0);

    //* Enable the RX and TX PDC transfer requests
	pPDC->PDC_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextRx
//* \brief Set the next receive transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetNextRx (
	AT91PS_PDC pPDC,     // \arg pointer to a PDC controller
	char *address,       // \arg address to the next bloc to be received
	unsigned int bytes)  // \arg number of bytes to be received
{
	pPDC->PDC_RNPR = (unsigned int) address;
	pPDC->PDC_RNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextTx
//* \brief Set the next transmit transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetNextTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TNPR = (unsigned int) address;
	pPDC->PDC_TNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetRx
//* \brief Set the receive transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetRx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be received
	unsigned int bytes)    // \arg number of bytes to be received
{
	pPDC->PDC_RPR = (unsigned int) address;
	pPDC->PDC_RCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetTx
//* \brief Set the transmit transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TPR = (unsigned int) address;
	pPDC->PDC_TCR = bytes;
}

unsigned char Check_Write_Important(unsigned int addr,unsigned char Isinchip,unsigned int addr_Inchip)
{
	int i=0;
	unsigned int length;
	unsigned short code=0;
	unsigned short* t_temp;

	length=*(unsigned int *)(addr+0x10);
	if (Isinchip==true)//片内
	{
		t_temp=(unsigned short *)(addr_Inchip);//判断不用了
	}
	else//不是片内的
	{
		t_temp=(unsigned short *)(addr+0x200);
	}

	if (length>0x3800)
	{
		AT91F_DBGU_Printk("code too long may without head");
		return error;
	}
	for (i=0;i<length;i++)//只算了部分
	{
		code=code+(*t_temp);
		t_temp++;			
	}
	
	t_temp=(unsigned short*)(addr);
	for (i=0;i<0x100;i++)
	{
		code=code+(*t_temp);
		t_temp++;
	}

	if (code==0)
	{
		AT91F_DBGU_Printk("check code right");					
		return right;
	}
	else
	{
		AT91F_DBGU_Printk("check code wrong");					
		return error;	
	}
}


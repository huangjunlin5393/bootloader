/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about USART.  
 * Author: Zhaoym
 * Created: 2008/03/26
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Usart.h"
#include "Dbgu.h"
//------------------------------------------------------------------------------
//         variables
//------------------------------------------------------------------------------
static char Uart0RecBuf[100];	/*Uart0 rec Buf for driver*/

//------------------------------------------------------------------------------
/// Fucntion Name:Usart_irq_handler
/// Aim:Usart0 Interrupt function.
/// Input:None;Output:None
//------------------------------------------------------------------------------
void Usart_irq_handler(void)
{
	
}
//------------------------------------------------------------------------------
/// Usart0 initiate.
/// 1.
/// 
//------------------------------------------------------------------------------
void uart0_init(void)
{
    // Configure PIO controllers to periph mode
    static const Pin pins[]  = {PINS_USART0};
    PIO_Configure(pins, PIO_LISTSIZE(pins));
    
    // 1.enable the clock of the UART0
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_US0);
    // 2.Usart Configure
    AT91F_US_Configure(BOARD_MCK,AT91C_US_ASYNC_MODE,USART_BAUD_RATE,0);
    // 3.Enable usart rec and tran
    AT91C_BASE_US0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
    // 4.open Usart interrupt,setup int process function
    AIC_ConfigureIT(AT91C_ID_US0,USART_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,Usart_irq_handler);
    AIC_EnableIT(AT91C_ID_US0);
    AT91F_PDC_Open(AT91C_BASE_PDC_US0);
    AT91C_BASE_US0->US_RPR = (unsigned int) Uart0RecBuf;
    AT91C_BASE_US0->US_RCR = UartLen;
    
    //5.Enable Interrupt
    AT91C_BASE_US0->US_IER=AT91C_US_RXBUFF;
}
//------------------------------------------------------------------------------
/// Usart configure
/// Input:mainClock,mode,baudRate,timeguard
/// Output:None
//------------------------------------------------------------------------------
void AT91F_US_Configure (
	unsigned int mainClock,  // \arg peripheral clock
	unsigned int mode ,      // \arg mode Register to be programmed
	unsigned int baudRate ,  // \arg baudrate to be programmed
	unsigned int timeguard ) // \arg timeguard to be programmed
{
    //* Disable interrupts
    AT91C_BASE_US0->US_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
    AT91C_BASE_US0->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS ;

    //* Define the baud rate divisor register
    AT91C_BASE_US0->US_BRGR = AT91F_US_Baudrate(mainClock, baudRate);

    //* Write the Timeguard Register
    AT91C_BASE_US0->US_TTGR = timeguard ;

    //* Clear Transmit and Receive Counters
    AT91F_PDC_Open((AT91PS_PDC) &(AT91C_BASE_US0->US_RPR));

    //* Define the USART mode
    AT91C_BASE_US0->US_MR = mode  ;

}

void SendToUart0( char *data ,unsigned char length)
{
	unsigned char i;
	for(i=0;i<length;i++)
	{
		uart0_putc( *data++ );
	}
}

int uart0_putc(int ch) 
{
	while (!(AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY));   /* Wait for Empty Tx Buffer */
	return (AT91C_BASE_US0->US_THR = ch);                 /* Transmit Character */
}

void GetFromUart0( char *data ,unsigned char length)
{
	unsigned char i;
	for(i=0;i<length;i++)
	{
		uart0_getc( *data++ );
	}
	SendDataToDbgu(data,length);
}

int uart0_getc(int ch) 
{
	while (!(AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY)); // receive data after 0->1  
	return (AT91C_BASE_US0->US_RHR = ch);                
}
//----------------------------------------------------------------------------
// function:    AT91F_US_Baudrate
// to Caluculate baud_value according to the main clock and the baud rate
//----------------------------------------------------------------------------
__inline unsigned int AT91F_US_Baudrate (
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

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Open
//* \brief Open PDC: disable TX and RX reset transfer descriptors, re-enable RX and TX
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_Open (
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
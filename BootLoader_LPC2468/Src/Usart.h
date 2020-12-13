/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the drivers and utilities for using USART.  
 * Author: Zhaoym
 * Created: 2008/03/26
 */
#ifndef USART_H
#define USART_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "PIO.h"
#include "aic.h"
#include "config.h"
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )
#define USART_BAUD_RATE 9600  // 9600 for GPS type of M12T
#define UartLen 20//for peripherial PDC Receive Counter Value
//------------------------------------------------------------------------------
//definitions of functions
//------------------------------------------------------------------------------
void uart0_init (void);
int uart0_putc(int ch);
void SendToUart0( char *data ,unsigned char length);
void GetFromUart0( char *data ,unsigned char length);
int uart0_getc(int ch);
void AT91F_US_Configure (
	unsigned int mainClock,  // \arg peripheral clock
	unsigned int mode ,      // \arg mode Register to be programmed
	unsigned int baudRate ,  // \arg baudrate to be programmed
	unsigned int timeguard ); // \arg timeguard to be programmed
unsigned int AT91F_US_Baudrate (
	const unsigned int main_clock, // \arg peripheral clock
	const unsigned int baud_rate);  // \arg UART baudrate
void AT91F_PDC_Open (AT91PS_PDC pPDC);       // \arg pointer to a PDC controller
void AT91F_PDC_SetNextRx (
	AT91PS_PDC pPDC,     // \arg pointer to a PDC controller
	char *address,       // \arg address to the next bloc to be received
	unsigned int bytes);  // \arg number of bytes to be received
void AT91F_PDC_SetNextTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes);    // \arg number of bytes to be transmitted
void AT91F_PDC_SetRx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be received
	unsigned int bytes);    // \arg number of bytes to be received
void AT91F_PDC_SetTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes);    // \arg number of bytes to be transmitted
	
#endif

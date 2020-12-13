/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the protocols
 * Author: Zhaoym
 * Created: 2008/03/27
 * Modified£ºLiulh 2009/11/08
 */
#ifndef DBGU_H
#define DBGU_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
//#include "PIO.h"
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )
#define UartLen 20//for peripherial PDC Receive Counter Value
#define Dbgu_BAUD_RATE 115200  // Baud Rate (to avoid float and cooperate with super teminal)
#define error 0
#define right 1
#define true 1
#define false 0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

void AT91F_DBGU_Printk(	char *buffer);
int AT91F_US_PutChar (AT91PS_USART pUSART,	int character );
int AT91F_US_TxReady (AT91PS_USART pUSART );     // \arg pointer to a USART controller


void AT91F_US_Configure (
    AT91PS_USART pUSART,
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
	
unsigned int AT91F_US_Baudrate (
	const unsigned int main_clock, // \arg peripheral clock
	const unsigned int baud_rate);  // \arg UART baudrate
void AT91F_US_SetTimeguard (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int timeguard); // \arg timeguard value
void AT91F_US_SetBaudrate (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int mainClock, // \arg peripheral clock
	unsigned int speed);     // \arg UART baudrate
void AT91F_DBGU_CfgPIO (void);
void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable);  // \arg PERIPH B to enable
unsigned char Check_Write_Important(unsigned int addr,unsigned char Isinchip,unsigned int addr_Inchip);
#endif


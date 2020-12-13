/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the drivers and utilities for using SSC.  
 * Author: Zhaoym
 * Created: 2008/04/16
 */
#ifndef SSC_H
#define SSC_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "Usart.h"
#include "Protocol.h"
#include "PIO.h"
#include "../include/math.h"

#define SSC_TxClk	8000000 //8Mbps
#define SSC_DIV (BOARD_MCK/SSC_TxClk/2)
#define SSC_PERIOD 30<<24
#define SSC_DATLEN  7<<0 //every (7+1)bit in one sample
#define SSC_DATNB  15<<8 //eveyr (7+1)sample in one frame
void SSC_Init(void);
void SSC_recvProcessing(void);
unsigned char SSC_recv_msg(void);
unsigned char SSC_send_msg(void);
void SSC_irq_handler(void);
void SSC_EnableTransmitter(void);
void SSC_DisableTransmitter(void);
void SSC_EnableReceiver(void);
void SSC_DisableReceiver(void);
void TransferToAdbuff(void);
#endif
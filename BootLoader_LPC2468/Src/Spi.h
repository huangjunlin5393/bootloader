/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the drivers and utilities for using SPI.  
 * Author: Zhaoym
 * Created: 2008/03/31
 */
#ifndef SPI_H
#define SPI_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "aic.h"
#include "config.h"
#include "PIO.h"
#include "../include/math.h"

//------------------------------------------------------------------------------
//         Macros
//------------------------------------------------------------------------------

/// Calculates the value of the SCBR field of the Chip Select Register given
/// MCK and SPCK.
#define SPID_SPCK 8000000
#define SPID_CSR_SCBR 	(unsigned int)((((BOARD_MCK) / (SPID_SPCK)) << 8) & AT91C_SPI_SCBR)

/// Calculates the value of the DLYBS field of the Chip Select Register given
/// the delay in ns and MCK.
#define SPID_delay_SPCK 100
#define SPID_CSR_DLYBS	((((((SPID_delay_SPCK) * ((BOARD_MCK) / 1000000)) / 1000) + 1)  << 16) & AT91C_SPI_DLYBS)

/// Calculates the value of the DLYBCT field of the Chip Select Register given
/// the delay in ns and MCK.
#define SPID_delay_BCT 1000
#define SPID_CSR_DLYBCT	((((((SPID_delay_BCT) / 32 * ((BOARD_MCK) / 1000000)) / 1000) + 1) << 24) & AT91C_SPI_DLYBCT)

//------------------------------------------------------------------------------
//        Definitions
//------------------------------------------------------------------------------
void SPI_Configure(unsigned char PCS);
unsigned char spi_FPGArecv_msg(void);
unsigned char spi_FPGAsend_msg(void);
void spi_ADsend_msg(unsigned char *sendbuf);
void spi_SYNsend_msg(unsigned char *sendbuf);
void SPI_irq_handler(void);
#endif
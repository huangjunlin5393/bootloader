/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about SPI.  
 * Author: Zhaoym
 * Created: 2008/03/31
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Spi.h"
#include "Protocol.h"
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
unsigned char spi_send_buf[112];

//------------------------------------------------------------------------------
//         Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  receive message from FPGA
//  this process has to be done immediately after the 5ms interrupt
//------------------------------------------------------------------------------
unsigned char spi_FPGArecv_msg(void)
{
	unsigned char retVal=0;
	//to receive 224Byte from SPI
    // Enable transmitter and receiver
	AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
	// Enable interrupt
	AT91C_BASE_SPI->SPI_IER=AT91C_SPI_RXBUFF;
	//start receiving
	/*AT91C_BASE_SPI->SPI_RPR = (unsigned int) spi_recv_buf;
    AT91C_BASE_SPI->SPI_RCR = 224/2;//transfer 16 bit every time*/
	
    return retVal;	
}


//------------------------------------------------------------------------------
//  send message to AD9863 to do the initialization.
//------------------------------------------------------------------------------
void spi_ADsend_msg(unsigned char *sendbuf)
{
	if (AT91C_BASE_SPI->SPI_TCR == 0) 
	{
        AT91C_BASE_SPI->SPI_TPR = (unsigned int) sendbuf;
        AT91C_BASE_SPI->SPI_TCR = 10;
        AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTEN;
    }
}

//------------------------------------------------------------------------------
//  send message to SYN
//------------------------------------------------------------------------------
void spi_SYNsend_msg(unsigned char *sendbuf)
{
	if (AT91C_BASE_SPI->SPI_TCR == 0) 
	{
        AT91C_BASE_SPI->SPI_TPR = (unsigned int) sendbuf;
        AT91C_BASE_SPI->SPI_TCR = 10;
        AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTEN;
    }
}

//------------------------------------------------------------------------------
//  send message to FPGA
//  this process has to be done immediately after the 5ms interrupt
//  there is no need to write PCS in SPI_TDR because FPGA equals to 0
//------------------------------------------------------------------------------
unsigned char spi_FPGAsend_msg(void)
{
	// Check if first bank is free
    if (AT91C_BASE_SPI->SPI_TCR == 0) 
	{
        AT91C_BASE_SPI->SPI_TPR = (unsigned int) spi_send_buf;
        AT91C_BASE_SPI->SPI_TCR = 112;
        AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTEN;
        return 1;
    }
    // Check if second bank is free
    else if (AT91C_BASE_SPI->SPI_TNCR == 0)
	{
        AT91C_BASE_SPI->SPI_TNPR = (unsigned int) spi_send_buf;
        AT91C_BASE_SPI->SPI_TNCR = 112/2;
        return 1;
    }
      
    // No free banks
    return 0;
}

//------------------------------------------------------------------------------
//  Initializes the Spid structure and the corresponding SPI hardware.
//  Input:the chip select number 3--FPGA;0--AD9863;1--SYN
//------------------------------------------------------------------------------
void SPI_Configure(unsigned char PCS)
{
    // Enable PIO
	static const Pin Spi_pins[] = {PINS_SPI,PIN_SPI_NPCS0,PIN_SPI_NPCS1,PIN_SPI_NPCS2};
	PIO_Configure(Spi_pins,PIO_LISTSIZE(Spi_pins));
	
	// Enable the SPI clock	
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_SPI);
    
    // Execute a software reset of the SPI
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SWRST;
    
    // Configure SPI_MR
    AT91C_BASE_SPI->SPI_MR = AT91C_SPI_MSTR|AT91C_SPI_PS_FIXED|AT91C_SPI_MODFDIS|(0x3<<16);
    // Configure SPI_CSR(we use SPI_CSR[0] only!)
    //AT91C_BASE_SPI->SPI_CSR[2] = SPID_CSR_SCBR;
	/*if(!PCS)//PCS==0 for AD9863
	{
		AT91C_BASE_SPI->SPI_CSR[0] = SPID_CSR_SCBR|AT91C_SPI_BITS_16;
	}
	else
	{
		AT91C_BASE_SPI->SPI_CSR[0] = SPID_CSR_SCBR|AT91C_SPI_BITS_8;		
	}*/
	AT91C_BASE_SPI->SPI_CSR[2] = SPID_CSR_SCBR|AT91C_SPI_BITS_8|(0x1<<24);
	// Enable the PDC transfer
	AT91F_PDC_Open (AT91C_BASE_PDC_SPI);
/*
	//Enable interrupt
	AIC_ConfigureIT(AT91C_ID_SPI,SPI_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,SPI_irq_handler);
    AIC_EnableIT(AT91C_ID_SPI);
	AT91C_BASE_SPI->SPI_IER = AT91C_SPI_RXBUFF;*/
    // Enable the SPI
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SPIEN;
    
}

//------------------------------------------------------------------------------
//  SPI Interrup processing function
//  
//------------------------------------------------------------------------------
void SPI_irq_handler(void)
{
	if (AT91C_BASE_SPI->SPI_IER & AT91C_SPI_RXBUFF) 
	{
		// Disable transmitter and receiver
		AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
		// Disable interrupt
		AT91C_BASE_SPI->SPI_IDR = AT91C_SPI_RXBUFF;
		//whether is receiving state
		/*if(!(flag_recvsend&bit_SSC_recv))
		{
			flag_recvsend |=bit_SSC_recv;//set  the bit

		}*/
	}
}

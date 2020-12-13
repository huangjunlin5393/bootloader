/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about SSC. 
 *           Receive SSC messages from FPGA. 
 * Author: Zhaoym
 * Created: 2008/04/16
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "SSC.h"
#include <string.h>
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
unsigned char SSC_recv_buf[192];// 20080710 for  tdd test
volatile signed short Ad_recv_buf[128];// 20080710 for  tdd test
char Ad_display_buf[256];
unsigned char SSC_send_buf[112];
extern unsigned char flag_recvsend;
//------------------------------------------------------------------------------
// SSC_Configure
//------------------------------------------------------------------------------
void SSC_Init(void)
{		
	// 1.Configure SSC PIOs	
    static const Pin pins[]  = {PINS_SSC_TX,PINS_SSC_RX};
    PIO_Configure(pins, PIO_LISTSIZE(pins));
	
	// 2.Configure PMC by enabling SSC clock 
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_SSC);

	AT91C_BASE_SSC->SSC_CR = AT91C_SSC_SWRST | AT91C_SSC_RXDIS | AT91C_SSC_TXDIS;
	// 3.Configure SSC Register
	////Write the Clock Mode Register

	AT91C_BASE_SSC->SSC_CMR = SSC_DIV;
    ////Write the Receive Clock Mode Register
	AT91C_BASE_SSC->SSC_RCMR = AT91C_SSC_CKS_RK|(0x1<<6);//|AT91C_SSC_CKI|AT91C_SSC_START_FALL_RF;|(0x1<<6)|SSC_PERIOD;
    ////Write the Transmit Clock Mode Register
	AT91C_BASE_SSC->SSC_TCMR =  AT91C_SSC_CKO_DATA_TX;
    ////Write the Receive Frame Mode Register
	AT91C_BASE_SSC->SSC_RFMR =  SSC_DATLEN|AT91C_SSC_MSBF;//|SSC_DATNB;
    ////Write the Transmit Frame Mode Register
	AT91C_BASE_SSC->SSC_TFMR =  SSC_DATLEN|AT91C_SSC_MSBF|AT91C_SSC_FSOS_LOW;//|SSC_DATNB;	

	
    // Clear Transmit and Receive Counters
	//Enable interrupt
	AIC_ConfigureIT(AT91C_ID_SSC,SSC_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,SSC_irq_handler);
    AIC_EnableIT(AT91C_ID_SSC);
	AT91F_PDC_Open(AT91C_BASE_PDC_SSC);
	SSC_recv_msg();

	/*//Enable receiver
	SSC_EnableReceiver();
	flag_recvsend |= bit_SSC_recv;
	*/
	memset(SSC_send_buf,0x3d,112);
	//SSC_EnableTransmitter();
	//* Reset receiver and transmitter
	AT91C_BASE_SSC->SSC_CR = 0;
	AT91C_BASE_SSC->SSC_CR = AT91C_SSC_TXEN|AT91C_SSC_RXEN;
	AT91C_BASE_SSC->SSC_IER = AT91C_SSC_RXBUFF;

}

//------------------------------------------------------------------------------
//  send SSC message to FPGA
//  this process has to be done immediately after the 5ms interrupt
//------------------------------------------------------------------------------
unsigned char SSC_send_msg(void)
{
	//wait untill last send complete
	//while(!(AT91C_BASE_SSC->SSC_SR&AT91C_SSC_TXBUFE));
	// Check if first bank is free
    if (AT91C_BASE_SSC->SSC_TCR == 0) 
	{
        AT91C_BASE_SSC->SSC_TPR = (unsigned int) SSC_send_buf;
        AT91C_BASE_SSC->SSC_TCR = 112;
        AT91C_BASE_SSC->SSC_PTCR = AT91C_PDC_TXTEN;
        return 1;
    }      
    // No free banks
    return 0;
}

//------------------------------------------------------------------------------
//  receive message from SSC,using PDC and interrupt
//------------------------------------------------------------------------------
unsigned char SSC_recv_msg(void)
{
	unsigned char retVal=0;
	if(!(flag_recvsend&bit_SSC_recv))//receiving data hasn't been processed
	{
	    // Check if the first bank is free
	    if (AT91C_BASE_SSC->SSC_RCR == 0)
		{
	        AT91C_BASE_SSC->SSC_RPR = (unsigned int) SSC_recv_buf;
	        AT91C_BASE_SSC->SSC_RCR = 192;
	        //AT91C_BASE_SSC->SSC_RCR = 12;
	        AT91C_BASE_SSC->SSC_PTCR = AT91C_PDC_RXTEN;
			retVal=1;
	    }
	}
    return retVal;	
}

//------------------------------------------------------------------------------
//  SSC Interrup processing function
//  after receiving 112Byte SSC message
//------------------------------------------------------------------------------
void SSC_irq_handler(void)
{
	/*if (AT91C_BASE_SSC->SSC_IMR & AT91C_SSC_RXBUFF) 
	{
		
		//whether is receiving state
		if(!(flag_recvsend&bit_SSC_recv))
		{
			flag_recvsend |=bit_SSC_recv;//set  the bit

		}
		
	}*/
    volatile unsigned long dummy;
	unsigned int imr=0;
	imr = AT91C_BASE_AIC->AIC_IMR;
	AT91C_BASE_AIC->AIC_IDCR = 0xffffffff;//disable AIC
	dummy = AT91C_BASE_SSC->SSC_SR;
	flag_recvsend |=bit_SSC_recv;
	AT91C_BASE_AIC->AIC_IECR = imr;//enable AIC
}

//------------------------------------------------------------------------------
/// Enables the transmitter of a SSC peripheral.
//------------------------------------------------------------------------------
void SSC_EnableTransmitter(void)
{
    AT91C_BASE_SSC->SSC_CR = AT91C_SSC_TXEN;
}

//------------------------------------------------------------------------------
/// Disables the transmitter of a SSC peripheral.
//------------------------------------------------------------------------------
void SSC_DisableTransmitter(void)
{
    AT91C_BASE_SSC->SSC_CR = AT91C_SSC_TXDIS;
}

//------------------------------------------------------------------------------
/// Enables the receiver of a SSC peripheral.
//------------------------------------------------------------------------------
void SSC_EnableReceiver(void)
{
    AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN;
}

//------------------------------------------------------------------------------
/// Disables the receiver of a SSC peripheral.
//------------------------------------------------------------------------------
void SSC_DisableReceiver(void)
{
    AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS;
}

//------------------------------------------------------------------------------
//  To processing the message receive from FPGA SPI
//------------------------------------------------------------------------------
void SSC_recvProcessing(void)
{
    if(SSC_recv_buf[2]==Type_FPGA_MCU)
    {
    	recvFGPA_processing(&SSC_recv_buf[4],SSC_recv_buf[3]);//to do signal message processing
    }	
    else if(CRC_Check(SSC_recv_buf))//if CRC is correct,get the context of message
    {
        switch(SSC_recv_buf[2])//according th the type	
        {
            case Type_MCUCode_Download:
                code_download(&SSC_recv_buf[4],SSC_recv_buf[3]);//to do the code downloading
            	break;

			case Type_Calibration:
                calibration_processing(&SSC_recv_buf[4],SSC_recv_buf[3]);//to do calibration processing
				break;

			case Type_Poll_Mcu:
				Poll_MCU_processing(&SSC_recv_buf[4]);			
				break;

			case Type_Config:
				config_processing(&SSC_recv_buf[4],SSC_recv_buf[3]);//to do configure processing
				break;

			case Type_maintenance:
				maintain_processing(&SSC_recv_buf[4],SSC_recv_buf[3]);//to do maintenance processing
				break;
				
			case Type_Data_Link_Init_Ack:
				
				break;

			case Type_WRRU_No_Req_Ack:
				
				break;

			case Type_WRRU_Para_Req_Ack:
				
				break;
			
            default:
            	break;	
        }
    }

	memset(SSC_recv_buf,0x00,sizeof(SSC_recv_buf));
}

void TransferToAdbuff(void)
{
	short i,j;
	/*SSC_recv_buf[0] = 0x12;
	SSC_recv_buf[1] = 0x3c;
	SSC_recv_buf[2] = 0x56;
	SSC_recv_buf[3] = 0x12;
	SSC_recv_buf[4] = 0x34;
	SSC_recv_buf[5] = 0x56;
	memset(SSC_recv_buf,0x5a,192);*/
	for(i=0,j=0;i<192;i++,j++)
	//for(i=0,j=0;i<12;i++,j++)
	{
		if(i%3 == 0)
		{
			Ad_recv_buf[j] = u2to1((unsigned char *)(SSC_recv_buf+i));
			Ad_recv_buf[j] = (Ad_recv_buf[j])>>4;
			Ad_recv_buf[j] &= 0x0fff;
		}
		else if(i%3 == 1)
		{
			Ad_recv_buf[j] = u2to1((unsigned char *)(SSC_recv_buf+i));
			Ad_recv_buf[j] = (Ad_recv_buf[j])<<4;
			Ad_recv_buf[j] = (Ad_recv_buf[j])>>4;
			Ad_recv_buf[j] &= 0x0fff;
			i++;
		}

	}
	for(i=0;i<128;i++)
	{
		u1to2(Ad_recv_buf[i],&(Ad_display_buf[i*2])); 
	}
}

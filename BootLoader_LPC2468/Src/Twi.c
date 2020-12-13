/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about TWI.
 *           TWI mode: master,no internal address, no AIC and no DMA! 
 *           And also for ADC.
 * Author: Zhaoym
 * Created: 2008/03/27
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Twi.h"
#include "math.h"
#include "Dbgu.h"
#include <string.h>

//------------------------------------------------------------------------------
//        Global variables
unsigned short AdcBuffer[Adc_SampleNum];
unsigned char TWIBuffer[TWI_Buf_Len];
unsigned int CurrentAver;
signed char Temp_Max = 85;
signed char Temp_Min = -40;
signed char temperature;
unsigned int Current_Max=4;//defulat value 20080504??
unsigned int Current_Min=1;//defulat value 20080504??
extern unsigned char flag_alarm_past;
extern unsigned char flag_alarm_now;
extern unsigned char flag_alarm_resumable;
extern unsigned char RFNum;

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         TWI Initiate
//------------------------------------------------------------------------------
void TWI_Init(void)
{
    // 1.Configure TWI PIOs	
    static const Pin pins[]  = {PIN_TWD,PIN_TWCK};
    PIO_Configure(pins, PIO_LISTSIZE(pins));
    // 2.Configure PMC by enabling TWI clock
    AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_TWI);
    // 3.Configure TWI in master mode;Set TWI Clock Waveform Generator Register
    TWI_Configure(AT91C_BASE_TWI,TWI_CLOCK ,BOARD_MCK);
}

//------------------------------------------------------------------------------
/// Configures a TWI peripheral to operate in master mode, at the given
/// frequency (in Hz). The duty cycle of the TWI clock is set to 50%.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param twck  Desired TWI clock frequency.
/// \param mck  Master clock frequency.
//------------------------------------------------------------------------------
void TWI_Configure(AT91S_TWI *pTwi,unsigned int twck, unsigned int mck)
{
    unsigned int ckdiv = 0;
    unsigned int cldiv;
    unsigned char ok = 0;
    
    //Disable interrupts
    pTwi->TWI_IDR = (unsigned int) -1;
    
    // Reset the TWI
    pTwi->TWI_CR = AT91C_TWI_SWRST;

    // Set master mode
    pTwi->TWI_CR = AT91C_TWI_MSEN;

    // Configure clock
    while (!ok) 
    {        
        cldiv = ((mck / (2 * twck)) - 3) / power(2, ckdiv);
        if (cldiv <= 255) {

            ok = 1;
        }
        else {

            ckdiv++;
        }
    }

    pTwi->TWI_CWGR = (ckdiv << 16) | (cldiv << 8) | cldiv;
}

//-----------------------------------------------------------------------------
/// Function Name: TWI_Read
/// Aim:Reads data from a slave on the TWI bus with no interrupt processing.
/// Return :0 if the transfer has been started; 
///         otherwise returns a TWI error code.
//-----------------------------------------------------------------------------
unsigned char TWI_Read(
    const AT91PS_TWI pTwid,/// Pointer to a Twid instance.
    unsigned char address,/// TWI slave address.
    unsigned int iaddress,/// Optional slave internal address.
    unsigned char isize,/// Internal address size in bytes.
    unsigned char *pData,///Data buffer for storing received bytes.
    unsigned int num)///Number of bytes to read.
{
	// Start read
    TWI_StartRead(pTwid, address, iaddress, isize);

    // Read all bytes, setting STOP before the last byte
    while(num > 0) 
	{
		// Last byte
        if(num == 1) 
		{
			pTwid->TWI_CR = AT91C_TWI_STOP;
         }

         // Wait for byte then read and store it
         while (!(pTwid->TWI_SR & AT91C_TWI_RXRDY));		 
         *pData++ = pTwid->TWI_RHR;
         num--;
     }

     // Wait for transfer to be complete
     while (!(pTwid->TWI_SR & AT91C_TWI_TXCOMP_MASTER));

	 return 0;
}

//------------------------------------------------------------------------------
/// Function Name: TWI_Write
/// Aim : Sends data to a slave on the TWI bus with no interrupt processing. 
/// Return :0 if the transfer has been started; 
///         otherwise returns a TWI error code.
//------------------------------------------------------------------------------
unsigned char TWI_Write(
    const AT91PS_TWI pTwid,/// Pointer to a Twid instance.
    unsigned char address,/// Slave address.
    unsigned int iaddress,/// Optional slave internal address.
    unsigned char isize,/// Number of internal address bytes.
    unsigned char *pData,///Data buffer to send.
    unsigned int num)///Number of bytes to send.
{
	TWI_StartWrite(pTwid, address, iaddress, isize, *pData++);
    num--;

    // Send all bytes
    while (num > 0) 
	{        
         // Wait before sending the next byte
         while (!(pTwid->TWI_SR & AT91C_TWI_TXRDY_MASTER));
		 pTwid->TWI_THR = *pData++;
         num--;
     }

     // Wait for actual end of transfer
     while (!(pTwid->TWI_SR & AT91C_TWI_TXCOMP_MASTER));

	 return 0;
}
//-----------------------------------------------------------------------------
/// Function Name: TWI_StarWrite
/// Aim: Starts a write operation on the TWI to access the selected slave.
///      A byte of data must be provided to start the write;
//-----------------------------------------------------------------------------
void TWI_StartWrite(
    AT91PS_TWI pTwi,	  //Pointer to an AT91S_TWI instance
    unsigned char address,//Address of slave to acccess on the bus
    unsigned int iaddress,//Optional slave internal address
    unsigned char isize,//Number of internal address bytes
    unsigned char byte)//First byte to send
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | (address << 15);//only 7bit addr!

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;

    // Write first byte to send
    pTwi->TWI_THR = byte;
}

//------------------------------------------------------------------------------
/// Fuction : TWI_StartRead
/// Aim : Set the master read mode and starts a read operation. 
//-----------------------------------------------------------------------------
void TWI_StartRead(
    AT91PS_TWI pTwi,	  //Pointer to a Twid instance
    unsigned char address,//Pointer to an AT91S_TWI instance
    unsigned int iaddress,//Optional internal address bytes
    unsigned char isize)  //Number of internal address bytes
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | AT91C_TWI_MREAD | (address << 15);//only 7bit addr!

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;

    // Send START condition
    pTwi->TWI_CR = AT91C_TWI_START;
}

//------------------------------------------------------------------------------
///Function Name : ADCInit_PDC
///Aim : To sample the current in AD7 32 times every 1 second. 
///      Using TIOA0 for hardware trigger. 
//------------------------------------------------------------------------------
void ADCInit_PDC(void)
{
    // 1.enable the clock of the ADC
    //AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_ADC);//commented 20080409
    
    // 2.Clear all previous setting and result 
    AT91C_BASE_ADC->ADC_CR = AT91C_ADC_SWRST;
    
    /*// 3.open ADC interrupt,setup interrupt process function
    AIC_ConfigureIT(AT91C_ID_ADC,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL|ADC_INTERRUPT_LEVEL,adc_irq_handler);	
    AIC_EnableIT( AT91C_ID_ADC);
    AT91F_PDC_Open(AT91C_BASE_PDC_ADC);
    
    // 4.Set the ADC buffer and counter
    AT91C_BASE_ADC->ADC_RPR = (unsigned int) AdcBuffer;
    AT91C_BASE_ADC->ADC_RCR = Adc_SampleNum;*/
    
    // 5.Set up by using ADC Mode register
    //AT91C_BASE_ADC->ADC_MR = (AT91C_ADC_SHTIM |AT91C_ADC_STARTUP | 
                            //AT91C_ADC_TRGSEL_TIOA0 | AT91C_ADC_TRGEN_EN );
	AT91C_BASE_ADC->ADC_MR = ADC_prescale<<8;
   // 6.Select the active channel
   AT91C_BASE_ADC->ADC_CHER = AT91C_ADC_CH7;
   
   /*//7.enable ADC interrupt
   AT91C_BASE_ADC->ADC_IER = AT91C_ADC_RXBUFF;*/
}
//------------------------------------------------------------------------------
//         ADC Interrupt function
//------------------------------------------------------------------------------
void adc_irq_handler(void)
{
	unsigned int status;
	unsigned char i;
	//* get ADC status register and active interrupt
	status = AT91C_BASE_ADC->ADC_SR ;

	if ( status & AT91C_ADC_RXBUFF)//REC INT
	{
		for (i=0;i<RFNum;i++)
		{
			CurrentAver+=(unsigned int)AdcBuffer[i];
		}
		CurrentAver = CurrentAver/RFNum;	    
	}
	AT91C_BASE_ADC->ADC_RPR = (unsigned int) AdcBuffer;
    AT91C_BASE_ADC->ADC_RCR = Adc_SampleNum;
}

void monitor_fiber(void)
{
	//TWIBuffer[0] = 0x00;
	//TWI_Write(AT91C_BASE_TWI,addr_fiber,0,0,TWIBuffer,1);
	
	TWI_Read(AT91C_BASE_TWI,addr_fiber,10,1,TWIBuffer,4);
	SendDataToDbgu((char *)TWIBuffer, 4);
	memset(TWIBuffer,0x00,sizeof(TWIBuffer));
/*
	flag_alarm_resumable |=bit_alarm_LightLink;//assume alarm at first	
	if(1)//if fiber is normal 
	{
		flag_alarm_now &=~bit_alarm_LightLink;//clear the bit
		// no alarm this time and no alarm last time
		if(!(flag_alarm_past & bit_alarm_LightLink))
		{
			flag_alarm_resumable &=~bit_alarm_LightLink;//clear the bit
		}		
	}
	else
	{
		flag_alarm_now |= bit_alarm_LightLink;//set the bit
	}	*/
}
void monitor_temperature(void)
{
	signed short temper;
	//TWIBuffer[0] = 0x00;
	//TWI_Write(AT91C_BASE_TWI,addr_tempsensor,0,0,TWIBuffer,1);
	TWI_Read(AT91C_BASE_TWI,addr_tempsensor,0,1,TWIBuffer,2);
	//SendMsgToDbgu("temperature is",strlen("temperature is"));
	//while(AT91C_BASE_DBGU->DBGU_TCR);
	SendDataToDbgu((char *)TWIBuffer, 2);
	//memset(TWIBuffer,0x00,sizeof(TWIBuffer));

	temper = u2to1((unsigned char * )TWIBuffer);
	temper = temper>>6;
	/*
	flag_alarm_resumable |=bit_alarm_temp;//assume alarm at first	
	if((temper> Temp_Min*4)&&(temper<Temp_Max*4))
	{
		flag_alarm_now &=~bit_alarm_temp;//clear the bit
		// no alarm this time and no alarm last time
		if(!(flag_alarm_past & bit_alarm_temp))
		{
			flag_alarm_resumable &=~bit_alarm_temp;//clear the bit
		}		
	}
	else
	{
		flag_alarm_now |= bit_alarm_temp;//set the bit
	}*/
}
void monitor_current(void)
{
	unsigned char i=0;
	for(i=0;i<Adc_SampleNum;i++)
	{
		//start adc
		AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;
		//read untill trasfer complete
		while(!((AT91C_BASE_ADC->ADC_SR)&AT91C_ADC_EOC7));
		AdcBuffer[i]=(unsigned short)(AT91C_BASE_ADC->ADC_CDR7);
	}
	SendDataToDbgu((char *)AdcBuffer, 10*2);
/*
	for (i=0;i<Adc_SampleNum;i++)
	{
		CurrentAver+=(unsigned int)AdcBuffer[i];
	}
	CurrentAver = 3.3*CurrentAver/RFNum/power(2,10);	 

	flag_alarm_resumable |=bit_alarm_current;//assume alarm at first
	if((CurrentAver> Current_Min)&&(CurrentAver<Current_Max))
	{
		flag_alarm_now &=~bit_alarm_current;//clear the bit
		// no alarm this time and no alarm last time
		if(!(flag_alarm_past & bit_alarm_current))
		{
			flag_alarm_resumable &=~bit_alarm_current;//clear the bit
		}		
	}
	else
	{
		flag_alarm_now |= bit_alarm_current;//set the bit
	}*/
}

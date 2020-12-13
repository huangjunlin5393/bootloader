/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about PIT.  
 *      And also Timer/Counter.
 *      And also the Extern 5ms interrtupt.
 * Author: Zhaoym
 * Created: 2008/03/27
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Pit.h"
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
unsigned short CNT_1ms;
extern unsigned char counter_cal_10ms;
extern unsigned char flag_recvsend;
extern unsigned char flag_extern_5ms;
extern unsigned char flag_adc_2s;
extern unsigned char flag_TWI_monitor;
extern unsigned char counter_1s;
//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Initialize the System timer for a period in u-second with a system clock
/// freq in MHz
/// \param period  Period in u-second.
/// \param pit_frequency  System clock frequency in MHz.
//------------------------------------------------------------------------------
void PIT_Init(unsigned int period)
{
    AT91C_BASE_PITC->PITC_PIMR = period;
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;
}
//------------------------------------------------------------------------------
/// Read PIT status register
//------------------------------------------------------------------------------
unsigned int PIT_GetStatus(void)
{
    return(AT91C_BASE_PITC->PITC_PISR);
}

//------------------------------------------------------------------------------
/// Read PIT CPIV and PICNT without ressetting the counters
//------------------------------------------------------------------------------
unsigned int PIT_GetPIIR(void)
{
    return(AT91C_BASE_PITC->PITC_PIIR);
}
//------------------------------------------------------------------------------
/// Read System timer CPIV and PICNT without ressetting the counters
//------------------------------------------------------------------------------
unsigned int PIT_GetPIVR(void)
{
    return(AT91C_BASE_PITC->PITC_PIVR);
}
//-----------------------------------------------------------------------------
/// configure PIT--the periodical time is 1ms
//-----------------------------------------------------------------------------
void configure_pit(void)
{
    //Enable peripherial clock
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_SYS);
    
    // Initialize and enable the PIT
    PIT_Init(BOARD_MCK *PIT_PERIOD/ 16  - 1);
    // Enable the interrupt on the pit
	
    //Disable the interrupt on the interrupt controller
    AIC_DisableIT(AT91C_ID_SYS);
    
    // Configure the AIC for PIT interrupts
    AIC_ConfigureIT(AT91C_ID_SYS, AT91C_AIC_SRCTYPE_POSITIVE_EDGE|PIT_INTERRUPT_LEVEL, pitc_handler);
    
    // Enable the interrupt on the interrupt controller
    AIC_EnableIT(AT91C_ID_SYS);

	PIT_EnableIT();
    
    //AT91C_BASE_AIC->AIC_ISCR = 0x1 << AT91C_ID_SYS ;//added 20080603

}

//-----------------------------------------------------------------------------
/// configure Extern 5ms Interrupt
//-----------------------------------------------------------------------------
void configure_5ms(void)
{
    //Disable the interrupt on the interrupt controller
    AIC_DisableIT(AT91C_ID_IRQ1);
    
    // Configure the AIC for PIT interrupts
    AIC_ConfigureIT(AT91C_ID_IRQ1, AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE|Extern_5msINTERRUPT_LEVEL, ExtInterrupt_5ms_handler);
    
    // Enable the interrupt on the interrupt controller
    AIC_EnableIT(AT91C_ID_IRQ1);	
}

//-----------------------------------------------------------------------------
/// Extern 5ms Interrupt function
/// Aim:to get the flag_extern_5ms value--1 for positive_edge;2 for negative_edge
///     positive_edge is to send FPGA SPI message;negative_edge is to receive.
//-----------------------------------------------------------------------------
void ExtInterrupt_5ms_handler(void)
{
    unsigned char SRC_type = 0;
    	
    //re-configure the AIC for 5ms
    AIC_DisableIT(AT91C_ID_IRQ1);//Disable the interrupt on the interrupt controller
    SRC_type = (((AT91C_BASE_AIC->AIC_SMR[AT91C_ID_IRQ1])&AT91C_AIC_SRCTYPE)>>5)&0x3;//to caculate the SRCTYPE
	//to set the ExtInterrupt flag

	if(SRC_type==3)//coming of transmitting 5ms timeslot
	{
		flag_extern_5ms |= bit_sending_5ms;
		if(flag_recvsend&bit_calibration)
		{
			counter_cal_10ms++;
		}
		if(flag_adc_2s<=200)
		{
			flag_adc_2s++;
		}
	}
	else if(SRC_type==5)//coming of receiving 5ms timeslot
	{
		flag_extern_5ms |= bit_recving_5ms;
	}
	SRC_type = (SRC_type+2)%4;
    AIC_ConfigureIT(AT91C_ID_IRQ1, (SRC_type<<5)|Extern_5msINTERRUPT_LEVEL, ExtInterrupt_5ms_handler);
    AIC_EnableIT(AT91C_ID_IRQ1);// Enable the interrupt on the interrupt controller

}

//-----------------------------------------------------------------------------
/// PIT interrupt function
/// to get three counter signal:1.30ms 
//-----------------------------------------------------------------------------
void pitc_handler(void)
{
	unsigned int pisr=0;
	unsigned int pivr=0;
	unsigned int imr=0;
	imr = AT91C_BASE_AIC->AIC_IMR;
	AT91C_BASE_AIC->AIC_IDCR = 0xffffffff;//disable AIC

	pisr = PIT_GetStatus();
	if(pisr)
	{
		pivr = PIT_GetPIVR();
		CNT_1ms++;
	}  
    /*if(CNT_1ms==30)//30ms to monitor the fiber module
    {
    	flag_TWI_monitor |=0x01;
    }
    if(CNT_1ms==1000)//1000ms to monitor the temperature
    {
    	CNT_1ms=0;
		flag_TWI_monitor |=0x01<<1;
    } */ 
    AT91C_BASE_AIC->AIC_IECR = imr;//enable AIC
}

//----------------------------------------------------------------------------
/// Enable PIT periodic interrupt
//----------------------------------------------------------------------------
void PIT_EnableIT(void)
{
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITIEN;
}

//------------------------------------------------------------------------------
/// Enable the PIT
//------------------------------------------------------------------------------
void PIT_Enable(void)
{
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;
}

//------------------------------------------------------------------------------
/// Disable PIT periodic interrupt
//------------------------------------------------------------------------------
void PIT_DisableIT(void)
{
    AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;
}

//------------------------------------------------------------------------------
/// Initialize TC0
/// Set the mode;config the Interrupt;
/// TC0 is to provide 1 second.
//------------------------------------------------------------------------------
void timer0_init(void)
{
    unsigned int mode = 0;
    mode = AT91C_TC_CLKS_TIMER_DIV5_CLOCK| //timer_clock5=MCK/1024
           AT91C_TC_WAVESEL_UP_AUTO| //RC compare trigger
           AT91C_TC_WAVE| //waveform mode
           AT91C_TC_ACPA_SET| //RA compare to set TIOA
           AT91C_TC_ACPC_CLEAR; //RC compare to clear TIOA         
           
    // 1.Init timer0
    AT91F_TC_Init(AT91C_BASE_TC0,mode,AT91C_ID_TC0);
    // Allow the user to use TIOA to output.
	/*AT91C_BASE_PIOB->PIO_ASR = AT91C_PB0_TIOA0;
	AT91C_BASE_PIOB->PIO_PDR = AT91C_PB0_TIOA0;*/ //20080620 to close TIOA0
	
    // 2.To have the TIOA wave (only for debug and there is no practical use) 
    AT91C_BASE_TC0->TC_RC = 1.1*BOARD_MCK/1024;
    AT91C_BASE_TC0->TC_RA = 1.1*BOARD_MCK/1024/2 ;

    // 3.Config the Interrupt(donnot enable interrupt)
    AIC_ConfigureIT ( AT91C_ID_TC0,TIMER0_INTERRUPT_LEVEL|AT91C_AIC_SRCTYPE_POSITIVE_EDGE,timer0_handler);
}

//------------------------------------------------------------------------------
/// Enable TC0
//------------------------------------------------------------------------------
void timer0_start(void)
{
    //  enable RC compare interrupt
    AT91C_BASE_TC0->TC_IER  = AT91C_TC_CPCS; 
    
    //  enable TC0 interrupt 
    AIC_EnableIT( AT91C_ID_TC0);
    
    // Enable the clock
    AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKEN ;
    
    // start the clock
    AT91C_BASE_TC0->TC_CCR |= AT91C_TC_SWTRG;
	AT91C_BASE_TC0->TC_CV = 0;
	counter_1s=0;
}

//------------------------------------------------------------------------------
/// TC0 interrupt handler.
/// Function: to caculate the time period user require.
//------------------------------------------------------------------------------
void timer0_handler(void)
{
    volatile unsigned long dummy;
	unsigned int imr=0;
	imr = AT91C_BASE_AIC->AIC_IMR;
	AT91C_BASE_AIC->AIC_IDCR = 0xffffffff;//disable AIC
    // Clear status bit
    dummy = AT91C_BASE_TC0->TC_SR;
	counter_1s++;
	AT91C_BASE_AIC->AIC_IECR = imr;//enable AIC

}

//------------------------------------------------------------------------------
/// Disable TC0
//------------------------------------------------------------------------------
void timer0_stop(void)
{
    //stop the clock
    //AT91C_BASE_TC0->TC_CMR |=AT91C_TC_CPCSTOP;
    // Disable TC0
    AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS ;
    //  disable TC0 interrupt 
    AIC_DisableIT (AT91C_ID_TC0);
    //  disable RC compare interrupt
    AT91C_BASE_TC0->TC_IDR  = AT91C_TC_CPCS;
	counter_1s = 0;
}

//------------------------------------------------------------------------------
///Function Name：AT91F_TC_Open
///Aim：Initialize Timer Counter Channel and enable the clock(donnot start!!)
///Input: TC Channel Descriptor Pointer;
///       mode:Timer Counter Mode
///       TimerId: Timer peripheral ID definitions
///Output: None
//------------------------------------------------------------------------------
void AT91F_TC_Init ( AT91PS_TC TC_pt, unsigned int Mode, unsigned int TimerId)
{
    	unsigned int dummy;

    	// enable the clock of the TIMER
	AT91C_BASE_PMC->PMC_PCER = 1<< TimerId;	
		
    	// 2.Disable the clock and the interrupts
	TC_pt->TC_CCR = AT91C_TC_CLKDIS ;
	TC_pt->TC_IDR = 0xFFFFFFFF ;

    	//Clear status bit
        dummy = TC_pt->TC_SR;
    	//Suppress warning variable "dummy" was set but never used
        dummy = dummy;

    	// 3.Set the Mode of the Timer Counter
	TC_pt->TC_CMR = Mode ;

}

//------------------------------------------------------------------------------
//         To Initialize WDTC
//------------------------------------------------------------------------------
void WDTC_Initialization(void)
{
    unsigned int WDT_Mode=0;
    // 1.enable the clock of the PIO 
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_SYS);
    
    // 2. set the mode //0x0A00:9s WDTC reset.
    WDT_Mode=(AT91C_WDTC_WDV &0x0A00)|(AT91C_WDTC_WDFIEN)|(AT91C_WDTC_WDRSTEN)|(AT91C_WDTC_WDD&(0x0A00<<16)) ; 
    AT91C_BASE_WDTC->WDTC_WDMR = WDT_Mode;//模式寄存器只能写一次,可以读多次，仿真模式不能使用看门狗
    
    // 3. reset the watchdog
	WDTC_Restart();
}

//------------------------------------------------------------------------------
//Aim: Restart WDTC.
//------------------------------------------------------------------------------
void WDTC_Restart(void)
{
	AT91C_BASE_WDTC->WDTC_WDCR = 0xA5000001;
}

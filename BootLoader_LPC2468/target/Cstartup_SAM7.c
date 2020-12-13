//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : Cstartup_SAM7.c
//* Object              : Low level initializations written in C for IAR
//*                       tools
//* Creation            : 12/Jun/04
//*
//* 
//* Modified 25/03/2008 by zhaoym
//*----------------------------------------------------------------------------


// Include the board file description
#include "board.h"
#include "PIO.h"
// The following functions must be write in ARM mode this function called directly
// by exception vector

//------------------------------------------------------------------------
//    Constants: Remap types
//        BOARD_FLASH - Flash is mirrored in the remap zone.
//        BOARD_RAM - RAM is mirrored in the remap zone.
//------------------------------------------------------------------------

void AT91F_Spurious_handler(void)
{
    
}
void AT91F_Default_IRQ_handler(void)
{
}
void AT91F_Default_FIQ_handler(void)
{
}



//------------------------------------------------------------------------------
/// Configures the EBI for NorFlash access at 48MHz.
/// \Param busWidth Bus width 
/// \WRRU18_DSB board use 16bit data bus
//------------------------------------------------------------------------------
void BOARD_ConfigureNorFlash(void)
{
    // Enable corresponding PIOs
    static const Pin pinsSdram[] = {BOARD_EXT_FPGA_CE,BOARD_EXT_FLASH_CE,BOARD_EXT_FLASH_OE,
								  BOARD_EXT_FLASH_WE,BOARD_EXT_FLASH_DataBus,
								  BOARD_EXT_FLASH_AddrBus}; 
//    static const Pin pinsSdram_m[]={BOARD_EXT_FLGAH_AddrBus_Multi};
	//const Pin Pin_Cs0 = BOARD_EXT_FLASH_CE;
	AT91C_BASE_EBI->EBI_CSA = 0;
	
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOC);
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOB);

	/*PIO_CfgOutput(&Pin_Cs0);
	PIO_Set(&Pin_Cs0);
	PIO_Clear(&Pin_Cs0);*/
    PIO_Configure(pinsSdram, PIO_LISTSIZE(pinsSdram));
// PIO_Configure_Muti(pinsSdram_m, PIO_LISTSIZE(pinsSdram_m));

	// Configure SMC //20080630 for SST39VF
    /*AT91C_BASE_SMC->SMC2_CSR[0] =
         ((unsigned int) 50 <<  0)           // set number of wait states 
       | AT91C_SMC2_WSEN            // wait state enabled
       | ((unsigned int) 5 <<  8)    // set Float Time Cycles 
       | AT91C_SMC2_BAT        // 1:16bit device connected over the bus
       | AT91C_SMC2_DBW_16     //16bit data bus
       | ((unsigned int) 0 << 15)    // Standard Read protocol required by the NAND Flash device
       | AT91C_SMC2_ACSS_STANDARD   // Standard address to chip select
       | ((unsigned int) 0 << 24) // 1 Read/Write Setup time required by the Nand Flash Device
       | ((unsigned int) 0 << 28); // 1 Read/Write Setup time required by the ECC controller
*/
	// 20080703 for Intel GT28F160C3BA110
	AT91C_BASE_SMC->SMC2_CSR[0] =
         ((unsigned int) 50 <<  0)           // set number of wait states 
       | AT91C_SMC2_WSEN            // wait state enabled
       | ((unsigned int) 5 <<  8)    // set Float Time Cycles 
       | AT91C_SMC2_BAT        // 1:16bit device connected over the bus
       | AT91C_SMC2_DBW_16     //16bit data bus
       | ((unsigned int) 0 << 15)    // Standard Read protocol required by the NAND Flash device
       | AT91C_SMC2_ACSS_STANDARD   // Standard address to chip select
       | ((unsigned int) 0 << 24) // 1 Read/Write Setup time required by the Nand Flash Device
       | ((unsigned int) 0 << 28); // 1 Read/Write Setup time required by the ECC controller


	//20090106 for FPGA
	AT91C_BASE_SMC->SMC2_CSR[7] =
         ((unsigned int) 3 <<  0)           // set number of wait states 
       | AT91C_SMC2_WSEN            // wait state enabled
       | ((unsigned int) 1 <<  8)    // set Float Time Cycles 
       | AT91C_SMC2_BAT        // 1:16bit device connected over the bus
       | AT91C_SMC2_DBW_16     //16bit data bus
       | ((unsigned int) 0 << 15)    // Standard Read protocol required by the NAND Flash device
       | AT91C_SMC2_ACSS_STANDARD   // Standard address to chip select
       | ((unsigned int) 0 << 24) // 1 Read/Write Setup time required by the Nand Flash Device
       | ((unsigned int) 0 << 28); // 1 Read/Write Setup time required by the ECC controller
	   

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LowLevelInit
//* \brief This function performs very low level HW initialization
//*        this function can be use a Stack, depending the compilation
//*        optimization mode
//*----------------------------------------------------------------------------
void AT91F_LowLevelInit( void)
{
 	int            i;
 	AT91PS_PMC     pPMC = AT91C_BASE_PMC;
    	//* Set Flash Waite sate
	//  Single Cycle Access at Up to 30 MHz, or 40
	//  if MCK = 47923200 I have 50 Cycle for 1 useconde ( flied MC_FMR->FMCN
	AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN)&(50 <<16)) | AT91C_MC_FWS_1FWS ;

	//* Set MCK at 47 923 200
    	// 1 Enabling the Main Oscillator:
        // SCK = 1/32768 = 30.51 uSeconde
    	// Start up time = 8 * 6 / SCK = 56 * 30.51 = 1,46484375 ms
       	pPMC->PMC_MOR = (( AT91C_CKGR_OSCOUNT & (48 <<8) | AT91C_CKGR_MOSCEN ));
       	
        // Wait the startup time
        while(!(pPMC->PMC_SR & AT91C_PMC_MOSCS));//20080708 for debug
        
	// 2 Checking the Main Oscillator Frequency (Optional)
	// 3 Setting PLL and divider(KHz):
		// - div by 8 Fin = 2,000 =(16,000 / 8)
		// - Mul 47+1: Fout =	96,000 =(2,000 *(47+1))
		// Field out NOT USED = 0
		// PLLCOUNT pll startup time esrtimate at : 0.844 ms
		// PLLCOUNT 28 = 0.000844 /(1/32768)
       pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & 0x08) |
                         (AT91C_CKGR_PLLCOUNT & (28<<8)) |
                         (AT91C_CKGR_MUL & (47<<16)));

        // Wait the startup time
        while(!(pPMC->PMC_SR & AT91C_PMC_LOCK));//20080708 for debug
        /* Wait for the master clock if it was already initialized */
    	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));//20080708 for debug
    	
	// 4. Selection of Master Clock and Processor Clock
        // select the PLL clock divided by 2
	pPMC->PMC_MCKR = AT91C_PMC_CSS_PLL_CLK | AT91C_PMC_PRES_CLK_2 ;
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));//20080708 for debug
//	pPMC->PMC_SCSR=AT91C_PMC_PCK|AT91C_PMC_PCK0|AT91C_PMC_PCK1|AT91C_PMC_PCK2;
	
	/* Initialize AIC
     	****************/

	AT91C_BASE_AIC->AIC_IDCR = 0xffffffff;//disable AIC

	AT91C_BASE_AIC->AIC_SVR[0] = (int) AT91F_Default_FIQ_handler ;
	for (i=1;i < 31; i++)
	{
	    AT91C_BASE_AIC->AIC_SVR[i] = (int) AT91F_Default_IRQ_handler ;
	}
	AT91C_BASE_AIC->AIC_SPU  = (int) AT91F_Spurious_handler ;
	
	// Unstack nested interrupts
    for (i = 0; i < 8 ; i++) 
    {
	 AT91C_BASE_AIC->AIC_EOICR = 0;
    }
    // Enable Debug mode
    //AT91C_BASE_AIC->AIC_DCR = AT91C_AIC_DCR_PROT;
    	
	//Watchdog Disable
    AT91C_BASE_WDTC->WDTC_WDMR= AT91C_WDTC_WDDIS; 
        
    // Remap£¿£¿£¿
//	BOARD_RemapRam();
	
	BOARD_ConfigureNorFlash();
}


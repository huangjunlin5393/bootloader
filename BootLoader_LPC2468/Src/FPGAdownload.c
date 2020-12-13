/***************************************************************************
FPGAdownload.c- Driver for the EP1K50FC256-3 chip./whx
modified by zy in 10/31/2003
modified by zhaoym 20080728
****************************************************************************/

#include "FPGAdownload.h"
//#include "fpgadata.h"
#include "NorFlash.h"
#include "config.h"
#include <stdio.h>
#if 0
extern 	char fpga_address[];
extern 	char fpga_addressend[];
#endif      
void init_fpga_gpio(void)
{	
/*	configure   pb as gpio,input or output
	
	 configure      output     PB24
	 dclk           output     PB25
	 data           output     PB23
	 nstatus        input      PB22
	 done           input      PB21
*/

	static const Pin pins_fpga_input[]  = {PIN_FPGA_NSTATUS,PIN_FPGA_DONE};
	static const Pin pins_fpga_output[]  = {PIN_FPGA_DATA,PIN_FPGA_DCLK,PIN_FPGA_CONFIGURE};
	PIO_Configure(pins_fpga_input,PIO_LISTSIZE(pins_fpga_input));
	PIO_Configure(pins_fpga_output,PIO_LISTSIZE(pins_fpga_output));
	
}

//int    fpga_download(unsigned char *data_ptr,int length)
int    fpga_download(void)	
{       
	int delay_count,i,j;
	flash_word fdata;
	flash_word *fpga_addr = (flash_word *)(INTEL_BASE+(addr_fpga_code1<<1));
    const Pin CONFIGURE = PIN_FPGA_CONFIGURE; 
	const Pin DCLK = PIN_FPGA_DCLK;
	const Pin DATA = PIN_FPGA_DATA;
	const Pin NSTATUS = PIN_FPGA_NSTATUS;
	const Pin DONE = PIN_FPGA_DONE;
	unsigned int imr=0;
	imr = AT91C_BASE_AIC->AIC_IMR;
	AT91C_BASE_AIC->AIC_IDCR = 0xffffffff;//disable AIC
	PIO_Clear(&CONFIGURE);       /*configure=0*/
	for( delay_count=0;delay_count<20;delay_count++);/*delay for some time*/
	PIO_Set(&CONFIGURE);       /*configure=1*/
	for( delay_count=0;delay_count<20*2;delay_count++);/*delay for some time*/
	for(i=0;i<0x2106d;i++)
	{
		//data=*data_ptr; 
		fdata=*fpga_addr;
        //for(j=0;j<8;j++)
        for(j=0;j<16;j++)
        {      
           	//for( delay_count=0;delay_count<30;delay_count++);       
			for( delay_count=0;delay_count<5;delay_count++);       
 			PIO_Clear(&DCLK); 
	//		if(((fdata>>j)&0x01)==0x01)    /* shift right,lsb first*/
				//SET_DATAPORT;
	//			PIO_Set(&DATA);
	//		else  
		        //CLR_DATAPORT;
				PIO_Clear(&DATA);
		
 			//for( delay_count=0;delay_count<30;delay_count++);
			for( delay_count=0;delay_count<5;delay_count++);       
 			//SET_DCLKPORT;
 			PIO_Set(&DCLK);
          }
          //data_ptr++;
		  fpga_addr++;
	}
	for( i=0;i<40;i++)  /*continue dclk for a period after download the fpga code */
	{
		for( delay_count=0;delay_count<5;delay_count++);       
 		//CLR_DCLKPORT;
 		PIO_Clear(&DCLK);
 		for( delay_count=0;delay_count<5;delay_count++);
 		//SET_DCLKPORT; 
 		PIO_Set(&DCLK);
	}
	    AT91C_BASE_AIC->AIC_IECR = imr;//enable AIC

     	if(!(PIO_GetInput(&DONE)))	 
     	return  0;     	
     	if(!(PIO_GetInput(&NSTATUS))) 
     	return 0;	
     	return 1;
}

/**********************************************************
To boot FPGA from GPIO of MPC860
***********************************************************/
int  fpga_boot(void)
{
	init_fpga_gpio();
	return(fpga_download());
	/*
       if(fpga_download(data_ptr,length)==0)
            printf("fpga program download fails!"); 
       else
            printf("fpga program download successes!"); 
	*/
}

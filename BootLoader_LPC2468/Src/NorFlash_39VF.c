/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of the off-chip Flash:SST39VF3201.EBI connecting to CS0.

 * Author: Zhaoym
 * Created: 2008/06/12
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "NorFlash_39VF.h"
#include "Dbgu.h" 
#include <stdlib.h>

void word_program_39VF(unsigned int address, unsigned short data)
{
    flash_word *addr;
    unsigned short detection_flag;
    unsigned int i, times;	
    flash_word word1,word2;
	i=0;
    addr = (flash_word *)((address<<1)|SST_BASE);	
    *(flash_word*)P_FIRST  = 0x00aa;
    *(flash_word*)P_SECOND = 0x0055;
    *(flash_word*)P_FIRST  = 0x00a0;
    (*addr) = data;
 
    times = 0;
    while(times!=2)
    {
        times = 0;
	for(i = 0;i<2;i++)
	{
	    detection_flag = (unsigned short)((*addr) & 0x80); //DQ7
	    if(detection_flag == (data & 0x80))
	    {
	       times++; //DQ7 equal halfword_data[7], program completion 
	    }   
	}		
   }/*
	for (i=0; i<0xffff; i++)
    {
        word1 = (unsigned short)(*addr);
        word2 = (unsigned short)(*addr);

        if ( (word1 & 0x40) == (word2 & 0x40) )
        {
            //status = TRUE;
            break;
        }
    }*/
}

 void block_erase_39VF(unsigned int block_addr)
 {
	flash_word i,word1,word2;
	flash_word times;
	flash_word *addr;
	addr = (flash_word *)((block_addr<<1)|SST_BASE);
	/*flash_word *chip_addr = (flash_word *)0x10000000;
	*((flash_word*)((addr1<<1)|0x10000000))  = 0x00aa;
	*((flash_word*)((addr2<<1)|0x10000000)) = 0x0055;
	*((flash_word*)((addr1<<1)|0x10000000))  = 0x0080;// for debug 20080619		
	*((flash_word*)((addr1<<1)|0x10000000))  = 0x00aa;
	*((flash_word*)((addr2<<1)|0x10000000)) = 0x0055;
	*((flash_word*)chip_addr) = 0x50;*/

	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*(flash_word*)P_FIRST  = 0x0080;
	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*addr = 0x50;
	times = 0;
	while(times!=2)                             
	{                                           
	  times = 0;
	  for(i = 0;i<2;i++)
	  {
	     if((*addr)& 0x80)
	     {  
	         times++; //DQ7 equal 1, erase completion
	     }
	  } 
	}
	times = 1;/*
	for (i=0; i<0xffff; i++)
    {
        word1 = (unsigned short)(*addr);
        word2 = (unsigned short)(*addr);

        if ( (word1 & 0x40) == (word2 & 0x40) )
        {
            //status = TRUE;
            break;
        }
    }
    */
 }

 //read length's flash_word
void ReadFlash(unsigned int address, unsigned char length)
{
	flash_word * dataptemp;
	flash_word * addtemp;
	flash_word data[5];
	unsigned int i;

	addtemp=(flash_word *)address;

	for(i=0;i<length;i++)
	{
		*dataptemp++=*addtemp;
		data[i] = *addtemp;
		addtemp++;
	}
	SendDataToDbgu((char *)dataptemp,length*2);
	
}

void test_39VF(void)
{
 	unsigned char p_block;
	unsigned short i;
	unsigned int p_word;
	volatile unsigned int wrong_addr[1000],wrong_data[1000];
	volatile unsigned int numtest;
	flash_word datatest;
	numtest = 0;

	memset((void *)wrong_addr, 0x0, sizeof(int)*1000);
	memset((void *)wrong_data, 0x0, sizeof(int)*1000);
	
	for(p_block=0;p_block<32;p_block++)
	{
		block_erase_39VF(p_block*0x8000);
		for(p_word=0;p_word<0x8000;p_word++)
		{
			word_program_39VF(p_block*0x8000+p_word,0x55aa);
			/*
			datatest = *(flash_word *)(SST_BASE+((p_block*0x8000+p_word)<<1));
			if(datatest != 0x55aa)
			{
				if(numtest<1000)
				{
					wrong_addr[numtest] = (p_block*0x8000+p_word);
				}
				numtest++;
			}*/
		}
		while(i<500)
			{
				i++;
			}
		i=0;
		for(p_word=0;p_word<0x8000;p_word++)
		{
			datatest = *(flash_word *)(SST_BASE+((p_block*0x8000+p_word)<<1));
			if(datatest != 0x55aa)
			{
				if(numtest<1000)
				{
					wrong_addr[numtest] = (p_block*0x8000+p_word);
					wrong_data[numtest] = datatest;
				}
				numtest++;
			}
			
		}
		/*while(i<1000)
			{
				i++;
			}
		i=0;*/	
	}
	p_word = 1;
}
void chip_erase_39VF(void)
{
	flash_word times;
	flash_word *addr;
	unsigned char i;
	addr = (flash_word *)SST_BASE;
	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*(flash_word*)P_FIRST  = 0x0080;
	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*(flash_word*)P_FIRST  = 0x0010;
	times = 0;
	while(times!=2)                             
	{                                           
	  times = 0;
	  for(i = 0;i<2;i++)
	  {
	     if((*addr)& 0x80)
	     {  
	         times++; //DQ7 equal 1, erase completion
	     }
	  } 
	}
}

void CFI_39VF(void)
{
	flash_word *addr;
	flash_word CFI_data[20];
	unsigned char i;
	//enter CFI query mode
	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*(flash_word*)P_FIRST  = 0x0098;
	
	//read CFI data at the given addresses 
	for(i=0;i<20;i++)
	{
		CFI_data[i] = *(flash_word *)(SST_BASE+((0x10+i)<<1));
	}
	//exit CFI mode and return to the Read mode 
	*(flash_word*)P_FIRST  = 0x00aa;
	*(flash_word*)P_SECOND  = 0x0055;
	*(flash_word*)P_FIRST  = 0x00f0;
}

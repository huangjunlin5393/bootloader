/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of the off-chip Flash:Intel-GT28F160C3BA110.EBI connecting.

 * Author: Zhaoym
 * Created: 2008/04/03
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "NorFlash.h"
#include <string.h>
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------

extern unsigned char flag_code_baseaddr;

unsigned char read_status_register(void)
{	
	unsigned char status;
	*(flash_word *)(0x00) = (flash_word)read_status;//don't care address
	status = (unsigned char)(*(flash_word *)(0x00));//read status register
	return status;
}

void clear_status_register(void)
{	
	*(flash_word *)(0x00) = (flash_word)clear_status;//don't care address
}

flash_word read_array_word(flash_word *addr_word)
{	
	*(flash_word *)(0x00) = (flash_word)read_array;
	return *addr_word;
}
//------------------------------------------------------------------------------
//Function :Program_Flash
//Aim: to program the code buff to the destinate address.
//Input:code_buf-address of code_buf;byte_num- 512 usually
//      flag_baseaddr(bit 0) - 0 for MCU code address 0;
//                   (bit 0) - 1 for MCU code address 1;
//					 (bit 1) - 0 for FPGA code address 0;
//					 (bit 1) - 1 for FPGA code address 1;
//Output:
//------------------------------------------------------------------------------
unsigned char program_flash(unsigned char *code_buf,
								  unsigned int byte_num,
								  const unsigned char flag_baseaddr)
{
	int offset_MCUcode0=0;
	int offset_MCUcode1=0;
	int offset_FPGAcode0=0;
	int offset_FPGAcode1=0;
	unsigned char status;
	unsigned char i;
	unsigned char pro_result;
	unsigned char err_num;
	//program setup
	for(i=0;i<byte_num/2;i++)
	{
		for(err_num=0;err_num<3;err_num++)
		{
			clear_status_register();
			*(flash_word *)(0x00) = (flash_word)program_setup;//don't care address

			if(flag_baseaddr&bit_MCUCode)
			{
				*(flash_word *)(addr_mcu_code0+offset_MCUcode1)=*(flash_word *)(code_buf+i*2);
				
			}
			else if(!(flag_baseaddr&bit_MCUCode))
			{
				*(flash_word *)(addr_mcu_code0+offset_MCUcode0)=*(flash_word *)(code_buf+i*2);
				
			}
			else if(flag_baseaddr&bit_FPGACode)
			{
				*(flash_word *)(addr_mcu_code0+offset_FPGAcode1)=*(flash_word *)(code_buf+i*2);
			
			}
			else if(!(flag_baseaddr&bit_FPGACode))
			{
				*(flash_word *)(addr_mcu_code0+offset_FPGAcode0)=*(flash_word *)(code_buf+i*2);
				
			}
			while(1)
			{
				status = read_status_register();
				if(status&bit_status_WSM)//wait untill WSM==1
				{
					break;
				}
			}
			pro_result = program_status_check();
			if(!(pro_result&bit_flag_err_program))
			{
				if(flag_baseaddr&bit_MCUCode)
				{
					if(read_array_word((flash_word *)(addr_mcu_code1+offset_MCUcode1))== *(flash_word *)(code_buf+i*2))
					{
						//success to program the word
						offset_MCUcode1 = offset_MCUcode1+i;
						break;
					}					
				}
				else if(!(flag_baseaddr&bit_MCUCode))
				{
					if(read_array_word((flash_word *)(addr_mcu_code0+offset_MCUcode0))== *(flash_word *)(code_buf+i*2))
					{
						//success to program the word
						offset_MCUcode0 = offset_MCUcode0+i;
						break;
					}					
				}
				else if(flag_baseaddr&bit_FPGACode)
				{
					if(read_array_word((flash_word *)(addr_fpga_code1+offset_FPGAcode1))== *(flash_word *)(code_buf+i*2))
					{
						//success to program the word
						offset_FPGAcode1 = offset_FPGAcode1+i;
						break;
					}				
				}
				else if(!(flag_baseaddr&bit_FPGACode))
				{
					if(read_array_word((flash_word *)(addr_fpga_code0+offset_FPGAcode0))== *(flash_word *)(code_buf+i*2))
					{
						//success to program the word
						offset_FPGAcode0 = offset_FPGAcode0+i;
						break;
					}					
				}			
			}

		}
		if(err_num<3)
		{
			clear_status_register();
		}
		else
		{
			dbgmsg_output("program word 3times wrong!\n\r",strlen("program word 3times wrong!\n\r"));
			return 0;
		}
	}
	return 1;
}

//
unsigned char erase_block(flash_word *block_addr )
{
	unsigned char status;
	unsigned char erase_result;
	//erase setup:write 20h to the addr within block to be erased
	*block_addr = (flash_word)erase_setup;
	//erase confirm:write 20h to the addr within block
	*block_addr = (flash_word)erase_confirm;	
	
	while(1)
	{
		status = read_status_register();
		if(status&bit_status_WSM)//wait untill WSM==1
		{
			break;
		}
	}
	erase_result = erase_status_check();
	clear_status_register();
	return erase_result;
}

// if an error detect,clear the status register before attemping retry or other error recovery.
unsigned char program_status_check(void)
{
	unsigned char status;
	unsigned char flag_flash_error;
	status = read_status_register();
	if(status&bit_status_VPPS)
	{
		flag_flash_error |=bit_flag_err_vpp;
	}
	else if(status&bit_status_PS)
	{
		flag_flash_error |=bit_flag_err_program;
	}
	else if(status&bit_status_BLS)
	{
		flag_flash_error |=bit_flag_err_abort;
	}
	return flag_flash_error;
}

unsigned char erase_status_check(void)
{
	unsigned char status;
	unsigned char flag_flash_error;
	status = read_status_register();
	if(status&bit_status_VPPS)
	{
		flag_flash_error |=bit_flag_err_vpp;
	}
	else if((status&bit_status_PS)||(status&bit_status_ES))
	{
		flag_flash_error |=bit_flag_err_cmd;
	}
	else if(status&bit_status_ES)
	{
		flag_flash_error |=bit_flag_err_erase;
	}
	else if(status&bit_status_BLS)
	{
		flag_flash_error |=bit_flag_err_abort;
	}
	return flag_flash_error;
}

void word_program_IntelGT28F(unsigned int address, unsigned short data)
{
    flash_word *addr,*pFW;
	flash_word retval,status;
    addr = (flash_word *)((address<<1)|INTEL_BASE);	
	pFW = (flash_word *)((address<<1)|INTEL_BASE);
	(*pFW) = 0x6060;
	(*pFW) = 0xd0d0;
	(*addr) = 0x4040;	
    (*addr) = data;
 	(*addr) = 0x7070;
	for(;;)
	{
		status = (*addr);
		if(status & 0x0080)
		{
			break;
		}
	}
	if(status & 0x004a)
	{
		retval =1;
	}
	*--pFW = 0xffff;
	(*addr) = 0x6060;
	(*addr) = 0x0101;
	(*addr) = 0xffff;
}

 void block_erase_IntelGT28F(unsigned int block_addr)
 {
	flash_word *addr;
	flash_word retval,status;
	addr = (flash_word *)((block_addr<<1)|INTEL_BASE);
	(*addr) = 0x6060;
	(*addr) = 0xd0d0;
	(*addr) = 0x2020;
	(*addr) = 0xd0d0;
	(*addr) = 0x7070;
	for(;;)
	{
		status = (*addr);
		if(status & 0x0080)
		{
			break;
		}
	}
	if(status & 0x004a)
	{
		retval =1;
	}
	(*addr) = 0xffff;
	(*addr) = 0x6060;
	(*addr) = 0x0101;
	(*addr) = 0xffff;
 }

 void test_IntelGT28F(void)
 {
 	unsigned char p_block;
	unsigned short i=0;
	unsigned int p_word;
	volatile unsigned int wrong_addr[1000],wrong_data[1000];
	volatile unsigned int numtest;
	flash_word datatest;
	numtest = 0;

	memset((void *)wrong_addr, 0x0, sizeof(int)*1000);
	memset((void *)wrong_data, 0x0, sizeof(int)*1000);
	/*
	for(p_block=0;p_block<8;p_block++)
	{
		block_erase_IntelGT28F(p_block*0x1000);
		for(p_word=0;p_word<0x1000;p_word++)
		{
			if((p_block == 0)&&(p_word == 0))
				p_word++;
			word_program_IntelGT28F(p_block*0x1000+p_word,0x55aa);
		}
		while(i<500)
			{
				i++;
			}
		i=0;
		for(p_word=0;p_word<0x1000;p_word++)
		{
			datatest = *(flash_word *)(INTEL_BASE+((p_block*0x1000+p_word)<<1));
			if(datatest != 0x55aa)
			{
				if(numtest<1000)
				{
					wrong_addr[numtest] = (p_block*0x1000+p_word);
					wrong_data[numtest] = datatest;
				}
				numtest++;
			}
			
		}
	}
	memset((void *)wrong_addr, 0x0, sizeof(int)*1000);
	memset((void *)wrong_data, 0x0, sizeof(int)*1000);*/
	for(p_block=0;p_block<31;p_block++)
	{
		block_erase_IntelGT28F(p_block*0x8000);
		for(p_word=0;p_word<0x8000;p_word++)
		{
			if((p_block == 0)&&(p_word == 0))
				p_word++;
			word_program_IntelGT28F(p_block*0x8000+p_word,0xa5a5);
		}
		while(i<500)
			{
				i++;
			}
		i=0;
		for(p_word=0;p_word<0x8000;p_word++)
		{
			datatest = *(flash_word *)(INTEL_BASE+((p_block*0x8000+p_word)<<1));
			if(datatest != 0xa5a5)
			{
				if(numtest<8000)
				{
					wrong_addr[numtest] = (p_block*0x8000+p_word);
					wrong_data[numtest] = datatest;
				}
				numtest++;
			}
			
		}
	}
	for(p_block=0;p_block<8;p_block++)
	{
		block_erase_IntelGT28F(0xf8000+p_block*0x1000);
		for(p_word=0;p_word<0x1000;p_word++)
		{
			word_program_IntelGT28F(0xf8000+p_block*0x1000+p_word,0x55aa);
		}
		while(i<500)
			{
				i++;
			}
		i=0;
		for(p_word=0;p_word<0x1000;p_word++)
		{
			datatest = *(flash_word *)(INTEL_BASE+((0xf8000+p_block*0x1000+p_word)<<1));
			if(datatest != 0x55aa)
			{
				if(numtest<1000)
				{
					wrong_addr[numtest] = (p_block*0x1000+p_word);
					wrong_data[numtest] = datatest;
				}
				numtest++;
			}
			
		}
	}
 }

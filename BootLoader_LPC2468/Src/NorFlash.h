/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: definitions of the off-chip Flash:Intel-GT28F160C3BA110.EBI connecting.

 * Author: Zhaoym
 * Created: 2008/04/03
 */
#ifndef NORFLASH_H
#define NORFLASH_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Board.h"
#include "Protocol.h"
#include "config.h"
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
#define program_setup	(flash_word)0x40
#define erase_setup		(flash_word)0x20
#define erase_confirm	(flash_word)0xD0
#define read_status		(flash_word)0x70
#define read_array		(flash_word)0xFF
#define clear_status	(flash_word)0x50
#define prg_ers_suspend (flash_word)0xB0
#define prg_ers_resume	 (flash_word)0xD0

#define bit_status_WSM	0x01<<7
#define bit_status_ESS	0x01<<6
#define bit_status_ES	0x01<<5
#define bit_status_PS	0x01<<4
#define bit_status_VPPS	0x01<<3
#define bit_status_PSS	0x01<<2
#define bit_status_BLS	0x01<<1
#define bit_status_R	0x01<<0

#define bit_flag_err_vpp 0x01<<0
#define bit_flag_err_cmd 0x01<<1
#define bit_flag_err_erase 0x01<<2
#define bit_flag_err_abort 0x01<<3
#define bit_flag_err_program	0x01<<4

//define the bit of FPGA code or MCU code
#define bit_FPGACode (0x01<<1)// 1 for addr_fpag_code1
#define bit_MCUCode	  (0x01<<0)// 1 for addr_mcu_code1
#define addr_mcu_code0	0x00000//32k word for MCU code
#define addr_fpga_code0 0x8000//32*14K word for FPGA
#define addr_mcu_code1  0x78000
#define addr_fpga_code1 0x80000

#define INTEL_BASE	0x10000000

//------------------------------------------------------------------------------
//         Definitions of Functions
//------------------------------------------------------------------------------
unsigned char program_flash(unsigned char *code_buf,unsigned int byte_num,const unsigned char flag_baseaddr);
unsigned char erase_block (flash_word *block_addr);
unsigned char program_status_check(void);
unsigned char erase_status_check(void);
unsigned char read_status_register(void);
void clear_status_register(void);
flash_word read_array_word(flash_word *addr_word);
void word_program_IntelGT28F(unsigned int address, unsigned short data);
void block_erase_IntelGT28F(unsigned int block_addr);
 void test_IntelGT28F(void);
#endif

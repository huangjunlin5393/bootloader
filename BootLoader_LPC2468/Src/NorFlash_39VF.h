/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: definitions of the off-chip Flash:SST39VF3201.EBI connecting to CS0.

 * Author: Zhaoym
 * Created: 2008/06/13
 */
#ifndef NORFLASH_39VF_H
#define NORFLASH_39VF_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Board.h"
#include "config.h"
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
//typedef unsigned short flash_word;
#define addr1 0x5555
#define addr2 0x2aaa
#define data1 (flash_word)0xaa
#define data2 (flash_word)0x55
#define data3 (flash_word)0x80
#define data4 (flash_word)0x50
#define data5 (flash_word)0xa0
#define SST_BASE	0x10000000
#define P_FIRST		(SST_BASE + (0x5555 << 1))
#define P_SECOND	(SST_BASE + (0x2aaa << 1))
void block_erase_39VF(unsigned int block_addr);
void word_program_39VF(unsigned int address, unsigned short data);
void ReadFlash(unsigned int address, unsigned char length);
void test_39VF(void);
void chip_erase_39VF(void);
#endif

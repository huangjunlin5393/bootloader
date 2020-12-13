/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support  -  ROUSSET  -
 * ----------------------------------------------------------------------------
 * Copyright (c) 2006, Atmel Corporation

 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution. 
 * 
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission. 
 * 
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef MATH_H
#define MATH_H

#define CRCSEED		0x0810	/* generator ploynomial */
#define MASK		0x0001

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned int min(unsigned int a, unsigned int b);
extern signed int abs(signed int value);
extern unsigned int power(unsigned int x, unsigned int y);
unsigned int dll_CalcCRC(unsigned char *start, unsigned char size );

typedef	unsigned char		UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned int		UINT32;

typedef	char				INT8;
typedef 	short				INT16;
typedef	int				INT32;

typedef	int				BOOL;
typedef	int				STATUS;

typedef 	void				VOID;

union	tagDataTypeSwitcher
{
	unsigned char		unit8[4];
	unsigned short	unit16[2];
	unsigned int		unit32;
};
typedef	union	tagDataTypeSwitcher	DataTypeSwitcher;

void u1to4( unsigned int  value,  char *pRetValue);
void u1to2(unsigned short value, char *pRetValue);
unsigned int u4to1(unsigned char *pRetValue);
unsigned short u2to1(unsigned char *pRetValue);
unsigned char CRC_Check(unsigned char *recvbuf);
#endif //#ifndef MATH_H


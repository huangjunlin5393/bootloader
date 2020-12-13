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

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "math.h"

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Returns the minimum value between two integers.
/// \param a  First integer to compare.
/// \param b  Second integer to compare.
//------------------------------------------------------------------------------
unsigned int min(unsigned int a, unsigned int b)
{
    if (a < b) {

        return a;
    }
    else {

        return b;
    }
}

//------------------------------------------------------------------------------
/// Returns the absolute value of an integer.
/// \param value  Integer value.
//------------------------------------------------------------------------------
signed int abs(signed int value)
{
    if (value < 0) {

        return -value;
    }
    else {

        return value;
    }
}

//------------------------------------------------------------------------------
/// Computes and returns x power of y.
/// \param x  Value.
/// \param y  Power.
//------------------------------------------------------------------------------
unsigned int power(unsigned int x, unsigned int y)
{
    unsigned int result = 1;
    while (y > 0) {

        result *= x;
        y--;
    }

    return result;
}

///-----------------------------------------------------------------------------
/// This function generates the 16 bit CRC	 16  15  2
///    using the following polynom: 				X + X + X + 1
///    Precise CRC computation algorithm definition:
///    CRC computation algorithm starts with zero
///    it treats the data msb first
///    CRC result is not complemented
/// Output: Returns: calculated CRC value
unsigned int dll_CalcCRC(unsigned char *start, unsigned char size )
{
    int i, j;
    char in;
    unsigned int remain = 0;
    for (i = 0 ; i < size; i++) 
    {
	in = *start++;
	for (j = 0 ; j < 8; j++) 
	{
	    if ((in^remain) & MASK) 
	    {
	 	remain ^= CRCSEED;
		remain >>=1;
	        remain |= 0x8000;
	    }
	    else
	    {
		remain >>=1;
	    }
	    in >>=1;
	}
    }
    return remain;		
}

void u1to4( unsigned int  value,  char *pRetValue)
{
	DataTypeSwitcher data_type;

	data_type.unit32 = value;	
	*pRetValue = data_type.unit8[3];
	*(pRetValue+1) = data_type.unit8[2];
	*(pRetValue+2) = data_type.unit8[1];
	*(pRetValue+3) = data_type.unit8[0];
}

void u1to2( unsigned short  value,  char *pRetValue)
{
	*pRetValue=(value&0xff00)>>8;
	*(pRetValue+1)=value&0xff;
}

unsigned int u4to1(unsigned char *pRetValue)
{
	unsigned int value=0;
	DataTypeSwitcher data_type;

	data_type.unit8[3]= *pRetValue;
	data_type.unit8[2]=*(pRetValue+1);
	data_type.unit8[1]=*(pRetValue+2);
	data_type.unit8[0]=*(pRetValue+3);
	value=data_type.unit32;
	return value;
}
unsigned short u2to1(unsigned char *pRetValue)
{
	unsigned short value=0;
	value = (pRetValue[0]<<8)|(pRetValue[1]);
	return value;
}
//------------------------------------------------------------------------------
//Function name : CRC_Check
//Aim: to check wether CRC of the input array is correct or not
//Input:Array address;
//Output:1--correct;0--incorrect
//------------------------------------------------------------------------------
unsigned char CRC_Check(unsigned char *recvbuf)
{
    unsigned int CalCrc=0;//CRC caculated according to the recvbuf 
    unsigned int RecCrc=0;//CRC value received	
    unsigned char Frame_len=recvbuf[3];
    unsigned char Crc_Check_len=Frame_len+4;
    unsigned char result_val=0; 
	  
    // to caculate the CRC value	   
    CalCrc=dll_CalcCRC(recvbuf,Crc_Check_len);
    // to convert the received CRC
    RecCrc=recvbuf[Crc_Check_len];
    RecCrc=((RecCrc<<8)|recvbuf[Crc_Check_len+1]);
    // to compare the two CRC values
    if(CalCrc==RecCrc)
    {
    	result_val=1;
    }
    return result_val;
}
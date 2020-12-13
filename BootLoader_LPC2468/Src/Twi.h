/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the drivers and utilities for using TWI. 
 *      And also for ADC. 
 * Author: Zhaoym
 * Created: 2008/03/27
 */
#ifndef TWI_H
#define TWI_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "PIO.h"
#include "aic.h"
#include "config.h"
#include "Usart.h"
#include "math.h"

//------------------------------------------------------------------------------
//        Definitions
//------------------------------------------------------------------------------
#define TWI_CLOCK 	100000 //in bit per sencond
#define addr_eeprom  0xa6//address of EEPROM in SYN
#define addr_tempsensor 0x92 //address of temperature sensor
#define addr_fiber  0xa0//address of  fiber module
#define Adc_SampleNum	32//number of sample
#define TWI_Buf_Len 5// 20080605 for debug fiber
#define ADC_prescale 47





//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------
extern void TWI_Init(void);
extern void TWI_Configure(AT91S_TWI *pTwi, unsigned int twck, unsigned int mck);
extern unsigned char TWI_Read(
    const AT91PS_TWI pTwid,
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize,
    unsigned char *pData,
    unsigned int num);

extern unsigned char TWI_Write(
    const AT91PS_TWI pTwid,
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize,
    unsigned char *pData,
    unsigned int num);
extern void TWI_StartRead(
    AT91PS_TWI pTwi,
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize);
extern void TWI_StartWrite(
    AT91PS_TWI pTwi,
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize,
    unsigned char byte);
extern void ADCInit_PDC(void);
void adc_irq_handler(void);
void monitor_fiber(void);
void monitor_temperature(void);
void monitor_current(void);
#endif //#ifndef TWI_H

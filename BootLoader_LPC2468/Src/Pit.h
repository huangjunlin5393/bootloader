/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the drivers and utilities for using PIT.  
 *      And also Timer/Counter.
 *      And also the Extern 5ms interrtupt.
 * Author: Zhaoym
 * Created: 2008/03/27
 */
#ifndef PIT_H
#define PIT_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "aic.h"
#include "config.h"
//------------------------------------------------------------------------------
//        Definitions
//------------------------------------------------------------------------------
#define PIT_PERIOD 0.001 // 0.001s
#define SAMPLING_FREQUENCY    30  /* Select the sampling frequency in hertz */
#define TIMER_VALUE  (BOARD_MCK/(1024*SAMPLING_FREQUENCY))/* You have to capture, in this formula, 2, 8, 32, 128 or 1024 according to TC_MCK_DIV */

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------
extern void PIT_Init(unsigned int period);
extern void configure_pit(void);
extern void configure_5ms(void);
extern void ExtInterrupt_5ms_handler(void);
extern void PIT_Enable(void);
extern void PIT_EnableIT(void);
extern void PIT_DisableIT(void);
extern unsigned int PIT_GetStatus(void);
extern unsigned int PIT_GetPIIR(void);
extern unsigned int PIT_GetPIVR(void);
void pitc_handler(void);
extern void timer0_init(void);
extern void timer0_start(void);
extern void timer0_stop(void);
void timer0_handler(void);
extern void AT91F_TC_Init ( AT91PS_TC TC_pt, unsigned int Mode, unsigned int TimerId);
void WDTC_Initialization(void);
void WDTC_Restart(void);
#endif //#ifndef PIT_H

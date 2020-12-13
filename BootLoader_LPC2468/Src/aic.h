/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: 
/// Methods and definitions for configuring interrupts using the Advanced
/// Interrupt Controller (AIC).
/// 
/// !Usage
/// -# Configure an interrupt source using AIC_ConfigureIT
/// -# Enable or disable interrupt generation of a particular source with
///    AIC_EnableIT and AIC_DisableIT.
//------------------------------------------------------------------------------  
 * Author: Zhaoym
 * Created: 2008/03/27
 */
#ifndef AIC_H
#define AIC_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
extern unsigned int AIC_ConfigureIT(unsigned int source,
                                   unsigned int mode,
                                   void (*handler)( void ));

extern void AIC_EnableIT(unsigned int source);

extern void AIC_DisableIT(unsigned int source);
#endif //#ifndef AIC_H
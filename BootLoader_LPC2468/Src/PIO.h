/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of methods and structures for using PIOs in a transparent way.  
 * Author: Zhaoym
 * Created: 2008/03/26
 */
#ifndef PIO_H
#define PIO_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \page "Pin types" 
/// This page lists the available types for a Pin instance (in its type field).
/// !Types
/// - PIO_PERIPH_A 
/// - PIO_PERIPH_B 
/// - PIO_INPUT 
/// - PIO_OUTPUT_0 
/// - PIO_OUTPUT_1 

/// The pin is controlled by the associated signal of peripheral A.
#define PIO_PERIPH_A                0
/// The pin is controlled by the associated signal of peripheral B.
#define PIO_PERIPH_B                1
/// The pin is an input.
#define PIO_INPUT                   2
/// The pin is an output and has a default level of 0.
#define PIO_OUTPUT_0                3
/// The pin is an output and has a default level of 1.
#define PIO_OUTPUT_1                4
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Pin attributes"
/// This page lists the valid values for the attribute field of a Pin instance.
/// !Attributes
/// - PIO_DEFAULT
/// - PIO_PULLUP
/// - PIO_DEGLITCH
/// - PIO_OPENDRAIN

/// Default pin configuration (no attribute).
#define PIO_DEFAULT                 (0 << 0)
/// The internal pin pull-up is active.
#define PIO_PULLUP                  (1 << 0)
/// The internal glitch filter is active.
#define PIO_DEGLITCH                (1 << 1)
/// The pin is open-drain.
#define PIO_OPENDRAIN               (1 << 2)
/// Calculates the size of a Pin instances array. The array must be local (i.e.
/// not a pointer), otherwise the computation will not be correct.
#define PIO_LISTSIZE(list)    (sizeof(list) / sizeof(Pin))

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Describes the type and attribute of one PIO pin or a group of similar pins.
typedef struct {
    /// Bitmask indicating which pin(s) to configure.
    unsigned int mask; 
    /// Pointer to the PIO controller which has the pin(s).
    AT91S_PIO    *pio;
    /// Peripheral ID of the PIO controller which has the pin(s).
    unsigned char id;
    /// Pin type (see "Pin types").
    unsigned char type;
    /// Pin attribute (see "Pin attributes").
    unsigned char attribute;
} Pin;

//------------------------------------------------------------------------------
//definitions of functions
//------------------------------------------------------------------------------
void PIO_Set(const Pin *pin );
void PIO_Clear(const Pin *pin);
void PIO_CfgOutput(const Pin *pin);
unsigned char PIO_Configure(const Pin *list, unsigned int size);
unsigned char PIO_GetInput(const Pin *pin);
unsigned char PIO_readversion(const Pin *list, unsigned int size);
#endif //#ifndef PIO_H

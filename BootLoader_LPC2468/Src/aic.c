/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about PIO.  
 * Author: Zhaoym
 * Created: 2008/03/27
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "aic.h"
#include "board.h"
//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Configures the interrupt associated with the given source, using the
/// specified mode and interrupt handler.
/// \param source  Interrupt source to configure.
/// \param mode  Triggering mode of the interrupt.
/// \param handler  Interrupt handler function.
//------------------------------------------------------------------------------
unsigned int AIC_ConfigureIT(unsigned int source,
                            unsigned int mode,
                            void (*handler)( void ))
{
	unsigned int oldHandler;
    unsigned int mask ;

    oldHandler = AT91C_BASE_AIC->AIC_SVR[source];

    mask = 0x1 << source ;
	// Disable the interrupt first
    AT91C_BASE_AIC->AIC_IDCR = mask;

    // Configure mode and handler
    AT91C_BASE_AIC->AIC_SVR[source] = (unsigned int) handler;
    AT91C_BASE_AIC->AIC_SMR[source] = mode;

    // Clear interrupt
    AT91C_BASE_AIC->AIC_ICCR = 1 << source;
	return oldHandler;
}

//------------------------------------------------------------------------------
/// Enables interrupts coming from the given (unique) source.
/// \param source  Interrupt source to enable.
//------------------------------------------------------------------------------
void AIC_EnableIT(unsigned int source)
{
    AT91C_BASE_AIC->AIC_IECR = 1 << source;
}

//------------------------------------------------------------------------------
/// Disables interrupts coming from the given (unique) source.
/// \param source  Interrupt source to enable.
//------------------------------------------------------------------------------
void AIC_DisableIT(unsigned int source)
{
    AT91C_BASE_AIC->AIC_IDCR = 1 << source;
}

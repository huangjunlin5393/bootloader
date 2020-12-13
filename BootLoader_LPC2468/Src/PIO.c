/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of functions about PIO.  
 * Author: Zhaoym
 * Created: 2008/03/26
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "PIO.h"
#include "board.h"

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------
/// \internal Returns the current value of a register.
#define READ(peripheral, register)          (peripheral->register)
/// \internal Modifies the current value of a register.
#define WRITE(peripheral, register, value)  (peripheral->register = value)

//------------------------------------------------------------------------------
/// Sets a high output level on one or more pin(s) (if configured as output(s)).
/// \param pin  Pointer to a Pin instance describing one or more pins.
//------------------------------------------------------------------------------
void PIO_Set(const Pin *pin)
{
    WRITE(pin->pio, PIO_SODR, pin->mask);
}

//------------------------------------------------------------------------------
/// Sets a low output level on one or more pin(s) (if configured as output(s)).
/// \param pin  Pointer to a Pin instance describing one or more pins.
//------------------------------------------------------------------------------
void PIO_Clear(const Pin *pin)
{
    WRITE(pin->pio, PIO_CODR, pin->mask);
}

//------------------------------------------------------------------------------
/// Config one or more pin(s) as output(s).
/// 1.PIO enable(PIO_PER register);2.Output enable(PIO_OER register)
/// \param pin  Pointer to a Pin instance describing one or more pins.
//------------------------------------------------------------------------------
void PIO_CfgOutput(const Pin *pin)
{
    // Disable interrupts
    WRITE(pin->pio, PIO_IDR, pin->mask);
    // Enable pull-up(s) if necessary
    if((pin->attribute & PIO_PULLUP) ? 1 : 0)
    {
    	WRITE(pin->pio, PIO_PPUER, pin->mask);
    }
    else
    {
    	WRITE(pin->pio, PIO_PPUDR, pin->mask);
    }
    // Configure pin as output    
    WRITE(pin->pio, PIO_PER, pin->mask);
    WRITE(pin->pio, PIO_OER, pin->mask);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Configures a list of Pin instances, which can either hold a single pin or a 
/// group of pins, depending on the mask value; all pins are configured by this 
/// function.
/// Returns 1 if the configuration has been performed successfully; otherwise 0.
/// \param list  Pointer to a list of Pin instances.
/// \param size  Size of the Pin list (see <PIO_LISTSIZE>).
//------------------------------------------------------------------------------
unsigned char PIO_Configure(const Pin *list, unsigned int size)
{
    // Configure pins
    while (size > 0) 
	{  
        switch (list->type) 
		{   
            case PIO_PERIPH_A:
                // Disable interrupts on the pin(s)
    			WRITE(list->pio, PIO_IDR, list->mask);
    			// Configure pin
    			WRITE(list->pio, PIO_ASR, list->mask);
    			WRITE(list->pio, PIO_PDR, list->mask);
                break;
    
            case PIO_PERIPH_B:
                // Disable interrupts on the pin(s)
    			WRITE(list->pio, PIO_IDR, list->mask);
    			// Configure pin
    			WRITE(list->pio, PIO_BSR, list->mask);
    			WRITE(list->pio, PIO_PDR, list->mask);
                break;
    
            case PIO_INPUT:
                // Disable interrupts
    			WRITE(list->pio, PIO_IDR, list->mask);
            	if((list->attribute & PIO_PULLUP) ? 1 : 0)
    			{
    		    	WRITE(list->pio, PIO_PPUER, list->mask);
    			}
    			else
    			{
    	            WRITE(list->pio, PIO_PPUDR, list->mask);
    			}
    			// Configure pin as iutput    
    			WRITE(list->pio, PIO_ODR, list->mask);
        		WRITE(list->pio, PIO_PER, list->mask);          
                break;
    
            case PIO_OUTPUT_0:
				// Disable interrupts
				WRITE(list->pio, PIO_IDR, list->mask);
			    // Enable pull-up(s) if necessary
			    if((list->attribute & PIO_PULLUP) ? 1 : 0)
			    {
			    	WRITE(list->pio, PIO_PPUER, list->mask);
			    }
			    else
			    {
			    	WRITE(list->pio, PIO_PPUDR, list->mask);
			    }
			    // Configure pin as output    
			    WRITE(list->pio, PIO_PER, list->mask);
			    WRITE(list->pio, PIO_OER, list->mask);
				break;
            case PIO_OUTPUT_1:               
                break;
    
            default: return 0;
        }

        list++;
        size--;
    }
    return 1;
}

//----------------------------------------------------------------------------
//PIO_GetInput
// 1.read the PIO_PDSR ;2. get the single pin is 1 or 0
//----------------------------------------------------------------------------
unsigned char PIO_GetInput(const Pin *pin) 
{
    unsigned int regst_val=0;
    regst_val=READ(pin->pio, PIO_PDSR);
    if(pin->mask&&regst_val)
    {
    	return 1;
    }
    else 
    {
    	return 0;
    }    
}
//----------------------------------------------------------------------------
//PIO_readversion
//Aim:Combine the 8 single bit value to get the whole version value(1byte)
//----------------------------------------------------------------------------
unsigned char PIO_readversion(const Pin *list, unsigned int size)
{
    unsigned char version_val=0;
    if (size==sizeof(unsigned char)*8)	
    {
    	while (size>0)
    	{
            version_val|=(PIO_GetInput(list)<<(size-1));
            list++;
            size--;
    	}    		
    }
    return version_val;
}

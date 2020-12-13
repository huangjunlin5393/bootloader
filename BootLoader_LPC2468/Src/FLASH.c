//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : Flash.c
//* Object              : Flash routine
//* Creation            : JPP   30/Jun/2004
//*----------------------------------------------------------------------------

// Include Standard files
#include "Flash.h"


//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Init
//* \brief Flash init
//*----------------------------------------------------------------------------
void AT91F_Flash_Init (void)
{
    //* Set Flash Waite sate
    //  Single Cycle Access at Up to 30 MHz
    //  if MCK = 47923200 I have 50 Cycle for 1 useconde ( flied MC_FMR->FMCN)
    //  = A page erase is performed before programming.
        AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN)&(50 <<16)) | AT91C_MC_FWS_1FWS ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Ready
//* \brief Wait the flash ready
//*----------------------------------------------------------------------------
void AT91F_Flash_Ready (void)
{
    //* Wait the and of command
          while ((AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY) != AT91C_MC_FRDY ) {};
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock_Status
//* \brief Get the MVM fild status
//*----------------------------------------------------------------------------
int AT91F_Flash_Lock_Status(void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_FSR_LOCK);
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock
//* \brief Wirte the Non Volatile Memory Bits and set at 0 FSR Bit = 1
//* \input page number (0-1023) a same region have some page
//* \output Region
//*----------------------------------------------------------------------------
int AT91F_Flash_Lock (unsigned int Flash_Lock_Page)
{
    //* set the Flasc controler base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Errase All command
        ptMC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_LOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) ;

    //* Wait the and of command
         AT91F_Flash_Ready();

  return (AT91F_Flash_Lock_Status());
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Unlock
//* \brief Clear the Non Volatile Memory Bits and set at 1 FSR bit=0
//* \input page number (0-1023) a same region have some page (16)
//* \output Region
//*----------------------------------------------------------------------------
int AT91F_Flash_Unlock(unsigned int Flash_Lock_Page)
{
    //* Write the Errase All command
        AT91C_BASE_MC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_UNLOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) ;

    //* Wait the and of command
        AT91F_Flash_Ready();

  return (AT91F_Flash_Lock_Status());
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Check_Erase
//* \brief Check the memory at 0xFF in 32 bits access
//*----------------------------------------------------------------------------
int AT91F_Flash_Check_Erase (unsigned int * start, unsigned int size)
{
	unsigned int i;
    //* Check if fash it erased
	for (i=0; i < (size/4) ; i++ )
	{
	    if ( start[i] != ERASE_VALUE ) return  0;
	}
	return 1 ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Erase_All
//* \brief Send command erase all flash
//*----------------------------------------------------------------------------
int AT91F_Flash_Erase_All(void)
{
    //* set the Flasc controler base address
        AT91PS_MC ptMC = AT91C_BASE_MC;
    //* Write the Errase All command
        ptMC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_ERASE_ALL ;
    //* Wait the and of command
        AT91F_Flash_Ready();
    //* Check the result
        return ( (ptMC->MC_FSR & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))==0) ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in 32 bits
//* \input Flash_Address: start at 0x0010 0000 size: in byte
//*----------------------------------------------------------------------------
int AT91F_Flash_Write( unsigned int Flash_Address ,int size ,unsigned int * buff)
{
    //* set the Flasc controler base address
    AT91PS_MC ptMC = AT91C_BASE_MC;
    unsigned int i, page;
    unsigned int * Flash;
    //* init flash pointer
        Flash = (unsigned int *) Flash_Address;
    //* Get the Flasg page number
        page = ((Flash_Address - (unsigned int)AT91C_IFLASH ) /FLASH_PAGE_SIZE_BYTE);
   //* copy the new value
	for (i=0; (i < FLASH_PAGE_SIZE_BYTE) & (size > 0) ;i++, Flash++,buff++,size-=4 ){
	//* copy the flash to the write buffer ensure that code generation
	    *Flash=*buff;
	}
    //* Write the Errase_All command
        ptMC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_START_PROG | (AT91C_MC_PAGEN & (page <<8)) ;
    //* Wait the end of command
         AT91F_Flash_Ready();
    //* Check the result
        if ( (ptMC->MC_FSR & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))!=0) return 0;
  return 1;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in byte
//* \input Start address (base=AT91C_IFLASH) size (in byte ) and buff address
//*----------------------------------------------------------------------------
int AT91F_Flash_Write_all( unsigned int Flash_Address ,int size ,unsigned int * buff)
{

    int   next, status;
    unsigned int  dest;
    unsigned int * src;

    dest = Flash_Address;
    src = buff;
    status = 1;

    while( (status == 1) & (size > 0) )
	{
        //* Check the size
        if (size <= FLASH_PAGE_SIZE_BYTE) next = size;
        else next = FLASH_PAGE_SIZE_BYTE;

        //* Unlock current sector base address - curent address by sector size
        AT91F_Flash_Unlock((dest - (unsigned int)AT91C_IFLASH ) /FLASH_PAGE_SIZE_BYTE);

        //* Write page and get status
        status = AT91F_Flash_Write( dest ,next ,src);
        // * get next page param
        size -= next;
        src += FLASH_PAGE_SIZE_BYTE/4;
        dest +=  FLASH_PAGE_SIZE_BYTE;
	}
    return status;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Status
//* \brief Get the MVM fild status
//*----------------------------------------------------------------------------
int AT91F_NVM_Status(void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_FSR_MVM);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Set
//* \brief Wirte the Non Volatile Memory Bits and set at 0 FSR Bit = 1
//*----------------------------------------------------------------------------
int AT91F_NVM_Set (unsigned char NVM_Number)
{
    //* set the Flasc controler base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Errase All command
        ptMC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_SET_GP_NVM | (AT91C_MC_PAGEN & (NVM_Number << 8) ) ;

    //* Wait the and of command
        AT91F_Flash_Ready();
  return (AT91F_NVM_Status());
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Clear
//* \brief Clear the Non Volatile Memory Bits and set at 1 FSR bit=0
//*----------------------------------------------------------------------------
int AT91F_NVM_Clear(unsigned char NVM_Number)
{
    //* set the Flasc controler base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Errase All command
        ptMC->MC_FCR = ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_CLR_GP_NVM | (AT91C_MC_PAGEN & (NVM_Number << 8) ) ;

    //* Wait the and of command
       AT91F_Flash_Ready();

  return (AT91F_NVM_Status());
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SET_Security_Status
//* \brief Wirte the Non Volatile Memory Bits and set at 0 FSR secutity = 1
//*----------------------------------------------------------------------------
int AT91F_SET_Security_Status (void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_SECURITY);
}

//*----------------------------------------------------------------------------
//* \fn AT91F_SET_Security
//* \brief Wirte the Non Volatile Memory Bits and set at 0 FSR secutity = 1
//*----------------------------------------------------------------------------
int AT91F_SET_Security (void)
{
	 //* write the flash
    //* Write the Errase All command
        AT91C_BASE_MC->MC_FCR = ( ((unsigned int) 0x5A << 24) | AT91C_MC_FCMD_SET_SECURITY ) ;

    //* Wait the and of command
       AT91F_Flash_Ready();

  return (AT91F_SET_Security_Status());
}



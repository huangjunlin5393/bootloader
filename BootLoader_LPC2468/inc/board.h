/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: To initlize the board, do some definitions
 * Author: Zhaoym
 * Created: 2008/03/25
*/
#ifndef Board_h
#define Board_h
#include "AT91SAM7SE32.h"
//------------------------------------------------------------------------------
/// \page "SAM7SE-EK - Operating frequencies"
/// This page lists several definition related to the board operating frequency
/// (when using the initialization done by board_lowlevel.c).
/// 
/// !Definitions
/// - BOARD_MAINOSC
/// - BOARD_MCK

/// Frequency of the board main oscillator.
#define BOARD_MAINOSC           16000000
/// Master clock frequency (when using AT91F_LowLevelInit.c).
#define BOARD_MCK               48000000

//------------------------------------------------------------------------------
/// \page "SAM7SE32_WRRU18_DSB board - PIO definitions"
/// This pages lists all the pio definitions contained in board.h. The constants
/// are named using the following convention: PIN_* for a constant which defines
/// a single Pin instance (but may include several PIOs sharing the same
/// controller), and PINS_* for a list of Pin instances.

/// DBGU pins (DTXD and DRXD) definitions.//PA9:DbgRXD PA10:DbgTXD
#define PINS_DBGU  {0x00000600, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// LED pin definition.//PA29:LED
#define PIN_LED  {1 << 29, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP}

/// USART0 TXD pin definition.//PA5~8:USART0
#define PIN_USART0_RXD  {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RXD pin definition.
#define PIN_USART0_TXD  {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RTS/CTS pin definition.(for handshake)
#define PIN_USART0_RTS  {1 << 7, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_CTS  {1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PINS_USART0	PIN_USART0_RXD,PIN_USART0_TXD,PIN_USART0_RTS,PIN_USART0_CTS

/// USART1 RXD1 pin definition.//PA21~28:USART1
#define PIN_USART1_RXD1  {1 << 21, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 TXT1 pin definition.
#define PIN_USART1_TXD1  {1 << 22, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 SCK1 pin definition.
#define PIN_USART1_SCK1  {1 << 23, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 RTS/CTS pin definition.(for handshake)
#define PIN_USART1_RTS1  {1 << 24, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_CTS1  {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 DCD1/DTR1/DSR1/Rl1 pin definition.(for Modem Signals Management)
#define PIN_USART1_DCD1  {1 << 26, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_DTR1  {1 << 27, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_DSR1  {1 << 28, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_Rl1  {1 << 28, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// SPI MISO pin definition.
#define PIN_SPI_MISO   {1 << 12, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
/// SPI MOSI pin definition.
#define PIN_SPI_MOSI   {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI SPCK pin definition.
#define PIN_SPI_SPCK   {1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI chip select 0 pin definition.
#define PIN_SPI_NPCS0  {1 << 11, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI chip select 1 pin definition.
#define PIN_SPI_NPCS1  {1 << 31, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI chip select 1 pin definition.
#define PIN_SPI_NPCS2  {1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// List of SPI pin definitions (MISO, MOSI & SPCK).
#define PINS_SPI       PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SPCK

/// SSC transmitter pins definition.(PA15~17)
#define PINS_SSC_TX  {0x00038000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/// SSC receiver pins definition.(PA18~20)
#define PINS_SSC_RX  {0x001c0000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// TWI pins definition.
#define PIN_TWD  {1<<3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_OPENDRAIN}
#define PIN_TWCK  {1<<4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_OPENDRAIN}

//EBI definition
/// Address for transferring command bytes to the norflash.
#define BOARD_EXT_FLASH_ADDR     0x10000000
//Chip Enable for Extern Flash
#define BOARD_EXT_FLASH_CE     {1 <<23 , AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
//Output Enable for Extern Flash
#define BOARD_EXT_FLASH_OE     {1 <<22 , AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
//Write Enable for Extern Flash
#define BOARD_EXT_FLASH_WE     {1 <<21 , AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
//Data Bus for Extern Flash(PC0~15)
#define BOARD_EXT_FLASH_DataBus {0x0000FFFF, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
//Address Bus for Extern Flash(PB1~17+PC16~18)
#define BOARD_EXT_FLASH_AddrBus {0x0003FFFE, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}, {0x00070000, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
//(PB1-10+PA11-17+PC16-18)
//hh#define BOARD_EXT_FLGAH_AddrBus {0x0007FF, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}, {0x0003F800, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT},{0x00070000, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
//#define BOARD_EXT_FLGAH_AddrBus_Multi {0x0001FC00, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}

//for FPGA donwload (PB21~25)
#define PIN_FPGA_CONFIGURE {1<<24, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_FPGA_DCLK {1<<25, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_FPGA_DATA {1<<23, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_FPGA_NSTATUS {1<<22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_FPGA_DONE {1<<21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
//for FPGA EBI Chip Enable 
#define BOARD_EXT_FPGA_CE     {1 <<20 , AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT}
//for FPGA RSV PB18
#define PINS_RSV_FPGA {1<<18, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_PULLUP}
//for FPGA IRQ0
#define PINS_FPGA_IRQ {1<<20, AT91C_BASE_PIOB, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_PULLUP}
//for FPGA IRQ1
#define PIN_FPGA_IRQ1 {1<<30, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
//for FPGA 10ms interupt (PB18~19)
#define PINS_10ms_interrupt {1<<19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_PULLUP}

//for watch dog
#define PIN_WDT {1<<31, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
//version 
#define PIN_Version_bit0 {1<<0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit1 {1<<1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit2 {1<<2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_PULLUP}

//#define PIN_Version_bit3 {1<<20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit4 {1<<26, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit5 {1<<27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit6 {1<<28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_Version_bit7 {1<<29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PINS_Version   PIN_Version_bit7,PIN_Version_bit6,PIN_Version_bit5,PIN_Version_bit4,PIN_Version_bit2,PIN_Version_bit1,PIN_Version_bit0                   

#endif

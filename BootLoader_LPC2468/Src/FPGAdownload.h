/***************************************************************************
FPGAdownload.h- head file for the EP1K50FC256-3 chip.
****************************************************************************/
#ifndef FPGADOWNLOAD_H
#define FPGADOWNLOAD_H


#include "PIO.h"
#include "Board.h"

void init_fpga_gpio(void);
//int   fpga_download(unsigned char *data_ptr,int length);
int  fpga_download(void);
int  fpga_boot(void);
#endif

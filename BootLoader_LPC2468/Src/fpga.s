        AREA    fpgafile, CODE, READONLY, ALIGN=2
        EXPORT  fpga_address
fpga_address
        INCBIN RRF_DSB.OUT
        
        EXPORT  fpga_addressend
fpga_addressend
	END
        

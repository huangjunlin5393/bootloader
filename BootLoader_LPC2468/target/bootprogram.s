;/***************************************************************
;
;*   版权所有(2003-2004)，北京信威通信技术有限公司
;
;*   文件名称：CPR.c
;
;*   功能描述：Low device driver for CPR
;
;*   数据结构：
;
;*   外部变量：no
;
;*   全局变量：no
;
;*   函    数：
;
;*   版    本：            				
;
;*   日    期：2004.10.02
;
;*   作    者：Liuting
;
;*   更改记录：
;
;****************************************************************/
	AREA    BOOTPROGRAM, CODE, READONLY

Mode_SVC        EQU     0x13 
I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled
	
    EXPORT	BootProgram
BootProgram
    MSR CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts 
    MOV PC,  r0   
    
    END

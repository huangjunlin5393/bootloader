;/***************************************************************
;
;*   ��Ȩ����(2003-2004)����������ͨ�ż������޹�˾
;
;*   �ļ����ƣ�CPR.c
;
;*   ����������Low device driver for CPR
;
;*   ���ݽṹ��
;
;*   �ⲿ������no
;
;*   ȫ�ֱ�����no
;
;*   ��    ����
;
;*   ��    ����            				
;
;*   ��    �ڣ�2004.10.02
;
;*   ��    �ߣ�Liuting
;
;*   ���ļ�¼��
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

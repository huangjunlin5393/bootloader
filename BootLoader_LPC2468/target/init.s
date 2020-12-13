;;; Copyright ARM Ltd 2001. All rights reserved.
;
; This module performs ROM/RAM remapping (if required), initializes stack 
; pointers and interrupts for each mode, and finally branches to __main in 
; the C library (which eventually calls main()).
;
; On reset, the ARM core starts up in Supervisor (SVC) mode, in ARM state, 
; with IRQ and FIQ disabled.
    
        INCLUDE		../inc/AT91SAM7SE32.inc

        AREA    Init, CODE, READONLY
        
; --- Set up if ROM/RAM remapping required

;                GBLL ROM_RAM_REMAP
;ROM_RAM_REMAP   SETL {TRUE} ; change to {FALSE} if remapping not required


; --- ensure no functions that use semihosting SWIs are linked in from the C library
        
; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

ARM_MODE_USER           EQU     0x10
ARM_MODE_FIQ            EQU     0x11
ARM_MODE_IRQ            EQU     0x12
ARM_MODE_SVC            EQU     0x13
ARM_MODE_ABORT          EQU     0x17
ARM_MODE_UNDEF          EQU     0x1B
ARM_MODE_SYS            EQU     0x1F

I_BIT                   EQU     0x80
F_BIT                   EQU     0x40
T_BIT                   EQU     0x20


Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F ; available on ARM Arch 4 and later

I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations

CM_ctl_reg      EQU     0x1000000C          ; Address of Core Module Control Register
Remap_bit       EQU     0x04                ; Bit 2 is remap bit of CM_ctl

; --- Amount of memory (in bytes) allocated for stacks

Len_FIQ_Stack    EQU     128
Len_IRQ_Stack    EQU     128
Len_ABT_Stack    EQU     0
Len_UND_Stack    EQU     0
Len_SVC_Stack    EQU     768
; Len_USR_Stack  EQU     1024

; Add lengths >0 for FIQ_Stack, ABT_Stack, UND_Stack if you need them.
; Offsets will be loaded as immediate values.
; Offsets must be 8 byte aligned.

Offset_FIQ_Stack         EQU     0
Offset_IRQ_Stack         EQU     Offset_FIQ_Stack + Len_FIQ_Stack
Offset_ABT_Stack         EQU     Offset_IRQ_Stack + Len_IRQ_Stack
Offset_UND_Stack         EQU     Offset_ABT_Stack + Len_ABT_Stack
Offset_SVC_Stack         EQU     Offset_UND_Stack + Len_UND_Stack
; Offset_USR_Stack       EQU     Offset_SVC_Stack + Len_SVC_Stack


        EXPORT  InitReset

InitReset

;  LDR     pc, =Instruct_2
;Instruct_2         

;  LDR     r0, =AT91C_BASE_MC
;  LDR     r1, =AT91C_MC_RCB
;  STR     r1, [r0, #MC_RCR]


; --- Initialize stack pointer registers

; Enter each mode in turn and set up the stack pointer

        IMPORT  top_of_stacks       ; defined in stack.s and located by scatter file
        LDR     r0, =top_of_stacks

        MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_FIQ_Stack

        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_IRQ_Stack

;       MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
;       SUB     sp, r0, #Offset_ABT_Stack

;       MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit ; No interrupts
;       SUB     sp, r0, #Offset_UND_Stack

        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_SVC_Stack



; --- Initialize critical IO devices
        ; ...


; --- Now change to User mode and set up User mode stack, if needed
;        MSR     CPSR_c, #Mode_USR:OR:I_Bit:OR:F_Bit ; No interrupts
;        SUB     sp, r0, #Offset_USR_Stack

        IMPORT   AT91F_LowLevelInit

;- minimum  C initialization

        ;ldr     r13,=0x00202000    ; temporary stack in internal Ram
	;ldr	r0,=AT91F_LowLevelInit
        ;mov     lr, pc
	;bx	r0
	BL      AT91F_LowLevelInit 
	    
    MSR     CPSR_c, #Mode_SVC                  ;Open interrupts

	                     
        IMPORT  __main

; --- Now enter the C code
        B       __main   ; note use B not BL, because an application will never return this way
        
        
;------------------------------------------------------------------------------
;- Manage exception
;---------------
;- The exception must be ensure in ARM mode
;------------------------------------------------------------------------------
       
;	EXPORT 		At91F_Default_Fiq_Handler
;AT91F_Default_FIQ_handler
;            b     AT91F_Default_FIQ_handler

;	EXPORT 		AT91F_Default_IRQ_handler
;AT91F_Default_IRQ_handler
;            b     AT91F_Default_IRQ_handler

;	EXPORT 		AT91F_Spurious_handler
;AT91F_Spurious_handler
;            b     AT91F_Spurious_handler
            
            
            EXPORT IRQ_Handler_Entry
IRQ_Handler_Entry

;- Manage Exception Entry
;- Adjust and save LR_irq in IRQ stack
            sub         lr, lr, #4
            stmfd       sp!, {lr}
;- Save and r0 in IRQ stack
            stmfd       sp!, {r0}

;- Write in the IVR to support Protect Mode
;- No effect in Normal Mode
;- De-assert the NIRQ and clear the source in Protect Mode
            ldr         r14, =AT91C_BASE_AIC
	    ldr         r0 , [r14, #AIC_IVR]
	    str         r14, [r14, #AIC_IVR]

;- Enable Interrupt and Switch in Supervisor Mode
           msr         CPSR_c, #ARM_MODE_SVC

;- Save scratch/used registers and LR in User Stack
            stmfd       sp!, { r1-r3, r12, r14}

;- Branch to the routine pointed by the AIC_IVR
            mov         r14, pc
            bx          r0

;- Restore scratch/used registers and LR from User Stack
            ldmia       sp!, { r1-r3, r12, r14}

;- Disable Interrupt and switch back in IRQ mode
            msr         CPSR_c, #I_BIT :OR: ARM_MODE_IRQ

;- Mark the End of Interrupt on the AIC
            ldr         r14, =AT91C_BASE_AIC
            str         r14, [r14, #AIC_EOICR]

;- Restore SPSR_irq and r0 from IRQ stack
            ldmia       sp!, {r0}

;- Restore adjusted  LR_irq from IRQ stack directly in the PC
            ldmia       sp!, {pc}^
            
            EXPORT FIQ_Handler_Entry
FIQ_Handler_Entry
;- Switch in SVC/User Mode to allow User Stack access for C code
; because the FIQ is not yet acknowledged

;- Save and r0 in FIQ_Register
        mov         r9,r0
        ldr         r8, =AT91C_BASE_AIC
        ldr         r0 , [r8, #AIC_FVR]
        msr         CPSR_c,#I_BIT | F_BIT | ARM_MODE_SVC
;- Save scratch/used registers and LR in User Stack
        stmfd       sp!, { r1-r3, r12, lr}

;- Branch to the routine pointed by the AIC_FVR
        mov         r14, pc
        bx          r0

;- Restore scratch/used registers and LR from User Stack
        ldmia       sp!, { r1-r3, r12, lr}

;- Leave Interrupts disabled and switch back in FIQ mode
        msr         CPSR_c, #I_BIT | F_BIT | ARM_MODE_FIQ

;- Restore the R0 ARM_MODE_SVC register
        mov         r0,r9

;- Restore the Program Counter using the LR_fiq directly in the PC
        subs        pc,lr,#4            
            
            
    IMPORT  bottom_of_heap       
   
    EXPORT __user_initial_stackheap
    
__user_initial_stackheap    

   	LDR   r0, =bottom_of_heap
   	ADD   r2, r0, #256
   	LDR   r3, =top_of_stacks    
   	ADD   r1, r3, #1024      
   	MOV   pc,lr        
    END


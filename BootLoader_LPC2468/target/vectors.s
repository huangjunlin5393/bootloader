;;; Copyright ARM Ltd 2001. All rights reserved.

        AREA Vect, CODE, READONLY

; These are example exception vectors and exception handlers
;
; Where there is ROM fixed at 0x0 (build_b), these are hard-coded at 0x0.
; Where ROM/RAM remapping occurs (build_c), these are copied from ROM to RAM.
; The copying is done automatically by the C library code inside __main.
;
; This version puts a branch to an Interrupt Handler (IRQ_Handler in int_handler.c) 
; in its exception vector table.


; *****************
; Exception Vectors
; *****************

; Note: LDR PC instructions are used here, though branch (B) instructions
; could also be used, unless the ROM is at an address >32MB.

        ENTRY

        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                             ; Reserved vector
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr

        IMPORT  IRQ_Handler_Entry             ; In int_handler.c
        IMPORT  InitReset           ; In init.s
        IMPORT  FIQ_Handler_Entry
        
Reset_Addr      DCD     InitReset
Undefined_Addr  DCD     Undefined_Handler
SWI_Addr        DCD     SWI_Handler
Prefetch_Addr   DCD     Prefetch_Handler
Abort_Addr      DCD     Abort_Handler
IRQ_Addr        DCD     IRQ_Handler_Entry
FIQ_Addr        DCD     FIQ_Handler_Entry


; ************************
; Exception Handlers
; ************************

; The following dummy handlers do not do anything useful in this example.
; They are set up here for completeness.

Undefined_Handler
        B       Undefined_Handler
SWI_Handler
        B       SWI_Handler     
Prefetch_Handler
        B       Prefetch_Handler
Abort_Handler
        B       Abort_Handler
;IRQ_Handler
;        B       IRQ_Handler
;FIQ_Handler
;         B       FIQ_Handler
        
        END


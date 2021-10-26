#include <xc.inc>

; When assembly code is placed in a psect, it can be manipulated as a
; whole by the linker and placed in memory.  
;
; In this example, barfunc is the program section (psect) name, 'local' means
; that the section will not be combined with other sections even if they have
; the same name.  class=CODE means the barfunc must go in the CODE container.
; PIC18's should have a delta (addressible unit size) of 1 (default) since they
; are byte addressible.  PIC10/12/16's have a delta of 2 since they are word
; addressible.  PIC18's should have a reloc (alignment) flag of 2 for any
; psect which contains executable code.  PIC10/12/16's can use the default
; reloc value of 1.  Use one of the psects below for the device you use:

psect   barfunc,local,class=CODE,delta=2 ; PIC10/12/16
; psect   barfunc,local,class=CODE,reloc=2 ; PIC18

global timer_hit, count
    
global _timer_function ; extern of bar function goes in the C source file
_timer_function:
    bsf LATC,5
    movlb  (timer_hit >> 8)
    btfss (timer_hit) & 0x7f,2
    goto timer_yes
    return
    
timer_yes:
    bcf LATC,5
    btfsc INTCON,1 ; TMR0IF
    goto timer_more_yes
    return

timer_more_yes:
    movlb  (count >> 8)
    incf (count) & 0x7f,0
    movwf (count) & 0x7f
    movlw 206
    movwf TMR0
    bcf INTCON,1 ; TMR0IF
    return
    
   


timer_hit: DB 0
count: DB 0
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "asm.h"

// reduced from mmu.h, x86 memory management
#define CR0_PE 0x00000001 
#define SEG_KCODE 1
#define SEG_KDATA 2

.code16  
.globl bootasm  
bootasm:
    cli                 // no interrupt
    xorw    %ax, %ax    // plain data seg 
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
                        // set cs in the future

seta20.1:		        // shut down A20
    inb     $0x64, %al
    testb   $0x2, %al
    jnz     seta20.1
    movb    $0xd1, %al
    outb    %al, $0x64

seta20.2:
    inb     $0x64, %al
    testb   $0x2, %al
    jnz     seta20.2
    movb    $0xdf, %al
    outb    %al, $0x60

    // push    %ebp

simple_detection:    
    xor     %ebx, %ebx      // Continuation
    mov     $0x9000, %di    // Pointer to buffer
    mov     $0x18, %ecx     // Buffer size

detec_loop:
    mov     $0xe820, %eax   // Function code
    mov     $0x0534D4150, %edx  // Signature
    int     $0x15 

    jc      end_detection
    add     $24, %edi
    test    %ebx, %ebx
    jne     detec_loop
    movw    %di, (0x8990)
end_detection:
    // pop     %ebp


protected_mode:

    lgdt    gdtdesc     // load global descrptor table
    movl    %cr0, %eax  // switch to protected mode
    orl     $CR0_PE, %eax
    movl    %eax, %cr0
                        // set cs
    ljmp   $(SEG_KCODE<<3), $start32
    
.code32
start32:
    movw    $(SEG_KDATA<<3), %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    movw    $0, %ax
    movw    %ax, %fs
    movw    %ax, %gs


    movl    $bootasm, %esp 
    call    bootmain

loop_boot:
    jmp     loop_boot

.p2align 2
gdt:
    SEG_NULLASM
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)
    SEG_ASM(STA_W, 0x0, 0xffffffff)
gdtdesc:
    .word (gdtdesc - gdt - 1)
    .long gdt

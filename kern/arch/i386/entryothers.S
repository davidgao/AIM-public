#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <segment.h>
#include <asm.h>
#include <cr.h>

#define ENT_OTHER_CODE_START 0x7000

.code16           
.globl entryother_start
entryother_start:
  cli   

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    (ENT_OTHER_CODE_START + entryother_gdtdesc - entryother_start)  
 
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmpl    $(SEG_KCODE<<3), $(ENT_OTHER_CODE_START + entrystart32 - entryother_start)

//PAGEBREAK!
.code32  # Tell assembler to generate 32-bit code now.
entrystart32:
  # Set up the protected-mode data segment registers

  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %ss                # -> SS: Stack Segment
  movw    $0, %ax                 # Zero segments not ready for use
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS

  # Turn on page size extension for 4Mbyte pages
  movl    %cr4, %eax
  orl     $(CR4_PSE), %eax
  movl    %eax, %cr4
  # Use entrypgdir as our initial page table
  movl    (ENT_OTHER_CODE_START - 12), %eax
  movl    %eax, %cr3
  # Turn on paging.
  movl    %cr0, %eax
  orl     $(CR0_PE|CR0_PG|CR0_WP), %eax
  movl    %eax, %cr0

  # Switch to the stack allocated by startothers()
  movl    (ENT_OTHER_CODE_START-4), %esp
  # Call mpenter()

  call	 *(ENT_OTHER_CODE_START-8)

  movw    $0x8a00, %ax
  movw    %ax, %dx
  outw    %ax, %dx
  movw    $0x8ae0, %ax
  outw    %ax, %dx
spin:
  jmp     spin


.p2align 2
entryother_gdt:
  SEG_NULLASM
  SEG_ASM(STA_X|STA_R, 0, 0xffffffff)
  SEG_ASM(STA_W, 0, 0xffffffff)


entryother_gdtdesc:
  .word   (entryother_gdtdesc - entryother_gdt - 1)
  .long   ENT_OTHER_CODE_START + entryother_gdt - entryother_start

.globl entryother_end
entryother_end:
  jmp entryother_end
  
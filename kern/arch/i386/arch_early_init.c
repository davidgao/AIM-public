/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "sys/types.h"

#include "aim/init.h"
#include "aim/boot.h"
#include "aim/mmu.h"
#include "aim/kalloc.h"
#include "segment.h"

extern uint32_t __bss_start_kern, __bss_end_kern;

void clear_bss_kern(){
    if (&__bss_end_kern > &__bss_start_kern)
        stosb(&__bss_start_kern, 0, 
            &__bss_end_kern - &__bss_start_kern);

}

static struct segdesc kern_gdt[NSEGS];

void arch_load_gdt() {
    
    kern_gdt[0] = SEG(0,0,0,0);
    kern_gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
    kern_gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
    kern_gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
    kern_gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
    kern_gdt[SEG_KCPU] = SEG(0,0,0,0);
    // lgdt a memory address
    lgdt((struct segdesc *)kern_gdt, sizeof(kern_gdt));
}

__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,

  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  [PDX(KERN_BASE)] = (0) | PTE_P | PTE_W | PTE_PS,
};

void set_cr_mmu();
void master_early_continue();

void arch_early_init(void)
{

    arch_load_gdt();   
    set_cr_mmu();
}

void arch_early_continue() {
    early_mm_init();
    master_early_continue();
}

void sleep1(){
    __asm__("cli;"::);
    while(1)
        __asm__("hlt;"::);
}
#ifndef _ARCH_INIT_H
#define _ARCH_INIT_H


#ifndef __ASSEMBLER__

#include <arch-mmu.h>

void lapicstartap(uchar apicid, uint addr);
void lapic_init(void);
void idt_init();
int cpunum(void);

void master_early_simple_alloc(void *start, void *end);
void get_early_end();
void page_alloc_init(addr_t start, addr_t end);
void master_later_alloc();
void trap_init();

void startothers(void);

#endif	//__ASSEMBLER__

#endif
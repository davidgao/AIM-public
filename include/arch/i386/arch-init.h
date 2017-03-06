#ifndef _ARCH_INIT_H
#define _ARCH_INIT_H


#ifndef __ASSEMBLER__

#include <arch-mmu.h>

void lapicstartap(uchar apicid, uint addr);
void lapic_init();
void idt_init();
int cpunum(void);
void mpinit(void);
void seginit();
void picinit();
void ioapic_init();

void master_early_simple_alloc(void *start, void *end);
void get_early_end();
void page_alloc_init(addr_t start, addr_t end);
void master_later_alloc();
void trap_init();

void startothers(void);

int quick_cpunum();
void panic_other_cpus();
void push_ipi(uint8_t intnum);
uint32_t __get_eip();

#endif	//__ASSEMBLER__

#endif
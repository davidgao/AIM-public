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

#include <sys/types.h>
#include <aim/console.h>
#include <aim/device.h>
#include <aim/early_kmmap.h>
#include <aim/init.h>
#include <aim/mmu.h>
#include <aim/panic.h>
#include <aim/kalloc.h>
#include <aim/pmm.h>
#include <aim/vmm.h>
#include <aim/initcalls.h>
#include <drivers/io/io-mem.h>
#include <drivers/io/io-port.h>
#include <platform.h>
#include <arch-init.h>
#include <arch-sync.h>
#include <mutex.h>
#include <asm.h>

void set_cr_mmu();

static inline
int early_devices_init(void)
{
#ifdef IO_MEM_ROOT
	if (io_mem_init(&early_memory_bus) < 0)
		return EOF;
#endif /* IO_MEM_ROOT */

#ifdef IO_PORT_ROOT
	if (io_port_init(&early_port_bus) < 0)
		return EOF;
#endif /* IO_PORT_ROOT */
	return 0;
}

typedef struct address_range_descriptor {
	uint64_t base;
	uint64_t length;
	uint64_t type;
} ARD;

#define ARD_ENTRY_ADDR 0x9000
#define ARD_COUNT_ADDR 0x8990
#define ARD_ENTRY_TARGET 3

static uint64_t __addr_base, __addr_length;

void get_mem_config() {
	// uint32_t n = (*(void **)ARD_COUNT_ADDR - (void *)ARD_ENTRY_ADDR) / sizeof(ARD);
	ARD *entry = (ARD *)ARD_ENTRY_ADDR;
	kprintf("Selected address range descriptor is :\n");
	kprintf("\t[%x%x, +%x%x], %x\n", 
		(uint32_t)entry[ARD_ENTRY_TARGET].base & 0xffffffff,
		(uint32_t)(entry[ARD_ENTRY_TARGET].base >> 32),
		(uint32_t)entry[ARD_ENTRY_TARGET].length & 0xffffffff,
		(uint32_t)(entry[ARD_ENTRY_TARGET].length >> 32), 
		(uint32_t)entry[ARD_ENTRY_TARGET].type
	);
	*(uint64_t *)((void *)&__addr_base) // - KERN_BASE) 
		= entry[ARD_ENTRY_TARGET].base;
	*(uint64_t *)((void *)&__addr_length) // - KERN_BASE) 
		= entry[ARD_ENTRY_TARGET].length;
}

__noreturn
void master_early_init(void)
{
	/* clear address-space-related callback handlers */
	early_mapping_clear();
	mmu_handlers_clear();
	/* prepare early devices like memory bus and port bus */

	if (early_devices_init() < 0)
		goto panic;
	/* other preperations, including early secondary buses */
	if (early_console_init(
		EARLY_CONSOLE_BUS,
		EARLY_CONSOLE_BASE,
		EARLY_CONSOLE_MAPPING
	) < 0)
		panic("Early console init failed.\n");
	kputs("Hello, world!\n");

	get_mem_config();
	arch_early_init();

	goto panic;

panic:
    sleep1();
	inf_loop();
}

extern addr_t *__early_buf_end;
void master_early_simple_alloc(void *start, void *end);
int page_allocator_init() {
	addr_t p_start = premap_addr(&__early_buf_end);
    if(__addr_base > p_start)
    	p_start = __addr_base;
    addr_t p_end = premap_addr(KERN_BASE + PHYSTOP);
    if(__addr_length + __addr_base < p_end)
    	p_end = __addr_length + __addr_base;
    page_alloc_init(p_start, p_end);
    kprintf("2. page allocator using [0x%p, 0x%p)\n", 
    	(void *)(uint32_t)p_start, (void *)(uint32_t)p_end
    );
    kprintf("\twith free space 0x%llx\n", get_free_memory());
    return 0;
}

void master_early_continue() {
    master_early_simple_alloc(
    	(void *)premap_addr((uint32_t)&__end), 
    	(void *)premap_addr(&__early_buf_end)
    );
    kprintf("1. early simple allocator using [0x%p, 0x%p)\n", 
    	(void *)premap_addr((uint32_t)&__end),
    	(void *)premap_addr(&__early_buf_end)
    );

	page_allocator_init();

    kprintf("3. later simple allocator depends on page allocator\n");
    master_later_alloc();

    mpinit();
    lapic_init();
    seginit();
    picinit();
    ioapic_init();
    do_initcalls();
    trap_init();
    
    startothers();

    void main_test();
    main_test();

}

void inf_loop() {
    while(1);
}

#define NAP 5
static lock_t lk = LOCK_INITIALIZER;
static semaphore_t sem = SEM_INITIALIZER(NAP);
static int critical_count = 300;
volatile static bool para_test_done = false;
void para_test() {
    semaphore_dec(&sem);
    while(sem.val > 0 && !para_test_done)   // sync all cpu to start together
        ;
    while(1) {
        spin_lock(&lk);             // enter critical section for countdown
        if(critical_count == 0) {   // about to finish the test
            kprintf("\ncpu %d done", quick_cpunum());
            semaphore_inc(&sem);    // submit work
            para_test_done = true;
            spin_unlock(&lk);       // unlock late for ordered output
            return;
        }
        kprintf("%d ", critical_count--);   // countdown
        spin_unlock(&lk);
    }
}

void main_test() {
    while(!(para_test_done && (sem.val == sem.limit)))  // every CPU submit
        ;
    kprintf("\n");
    panic("All processors finished para_test\n"); // panic all cpu
}
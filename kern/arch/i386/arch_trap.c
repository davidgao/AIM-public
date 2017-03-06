#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <arch-trap.h>
#include <arch-mmu.h>
#include <segment.h>
#include <aim/panic.h>
#include <aim/trap.h>
#include <aim/console.h>
#include <asm.h>
#include <proc.h>
#include <arch-init.h>

#define NIDT 256

static struct gatedesc idt[NIDT];
extern uint32_t vectors[];

__noreturn
void trap_return(struct trapframe *tf);

void idt_init() {
	for(int i=0; i < NIDT; ++i) {
		SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
	}
	SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
}

void trap(struct trapframe *tf) {
	long ans;
	if(tf->trapno == T_SYSCALL) {
		ans = handle_syscall(
			tf->eax, tf->ebx, tf->ecx, tf->edx,
			tf->esi, tf->edi, tf->ebp
		);
		tf->eax = ans;
		return;
	}
	if(tf->trapno >= T_IRQ0 && tf->trapno < T_IRQ0 + 32) {
    handle_interrupt(tf->trapno - T_IRQ0);
		return;
	}

  switch(tf->trapno) {
    case T_PANICALL_:
      local_panic("INT PANICALL: CPU %d panic\n", quick_cpunum());
      return;
    case T_SHOWEIP_:
      local_panic("INT SHOWEIP: CPU %d at 0x%x\n", quick_cpunum(), __get_eip());
      return;
  }

  kprintf("CPU %d Receive undefined trapno 0x%x\n", quick_cpunum(), tf->trapno);
	
  asm("hlt");

  panic("trap: Implement me\n");

}


void trap_init(void) {
	idt_init();	// prepare int vectors

	lidt((struct gatedesc *)idt, sizeof(idt));
}

uint32_t __get_eip() {
  asm volatile(
    "mov (%%esp), %%eax;"
    "ret;"
    ::
  );
  return 0; // to deceive compiler
}


// I/O Addresses of the two programmable interrupt controllers
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

#define IRQ_SLAVE       2       // IRQ at which slave connects to master

// Current IRQ mask.
// Initial IRQ mask has interrupt 2 enabled (for slave 8259A).
static ushort irqmask = 0xFFFF & ~(1<<IRQ_SLAVE);

static void
picsetmask(ushort mask)
{
  irqmask = mask;
  outb(IO_PIC1+1, mask);
  outb(IO_PIC2+1, mask >> 8);
}

void
picenable(int irq)
{
  picsetmask(irqmask & ~(1<<irq));
}

// Initialize the 8259A interrupt controllers.
void
picinit(void)
{
  // mask all interrupts
  outb(IO_PIC1+1, 0xFF);
  outb(IO_PIC2+1, 0xFF);

  // Set up master (8259A-1)

  // ICW1:  0001g0hi
  //    g:  0 = edge triggering, 1 = level triggering
  //    h:  0 = cascaded PICs, 1 = master only
  //    i:  0 = no ICW4, 1 = ICW4 required
  outb(IO_PIC1, 0x11);

  // ICW2:  Vector offset
  outb(IO_PIC1+1, T_IRQ0);

  // ICW3:  (master PIC) bit mask of IR lines connected to slaves
  //        (slave PIC) 3-bit # of slave's connection to master
  outb(IO_PIC1+1, 1<<IRQ_SLAVE);

  // ICW4:  000nbmap
  //    n:  1 = special fully nested mode
  //    b:  1 = buffered mode
  //    m:  0 = slave PIC, 1 = master PIC
  //      (ignored when b is 0, as the master/slave role
  //      can be hardwired).
  //    a:  1 = Automatic EOI mode
  //    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
  outb(IO_PIC1+1, 0x3);

  // Set up slave (8259A-2)
  outb(IO_PIC2, 0x11);                  // ICW1
  outb(IO_PIC2+1, T_IRQ0 + 8);      // ICW2
  outb(IO_PIC2+1, IRQ_SLAVE);           // ICW3
  // NB Automatic EOI mode doesn't tend to work on the slave.
  // Linux source code says it's "to be investigated".
  outb(IO_PIC2+1, 0x3);                 // ICW4

  // OCW3:  0ef01prs
  //   ef:  0x = NOP, 10 = clear specific mask, 11 = set specific mask
  //    p:  0 = no polling, 1 = polling mode
  //   rs:  0x = NOP, 10 = read IRR, 11 = read ISR
  outb(IO_PIC1, 0x68);             // clear specific mask
  outb(IO_PIC1, 0x0a);             // read IRR by default

  outb(IO_PIC2, 0x68);             // OCW3
  outb(IO_PIC2, 0x0a);             // OCW3

  if(irqmask != 0xFFFF)
    picsetmask(irqmask);
}
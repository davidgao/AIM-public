#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <proc.h>
#include <asm.h>
#include <segment.h>
#include <arch-init.h>
#include <arch-mmu.h>
#include <aim/console.h>
#include <aim/pmm.h>
#include <libc/string.h>


struct cpu cpus[NCPU];
int ismp;
int ncpu;
uchar ioapicid;


void
seginit(void)
{
  struct cpu *c;

  c = &cpus[cpunum()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);

  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 8, 0);

  lgdt(c->gdt, sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
}

// Common CPU setup code.
void
mpmain(void)
{
  //cprintf("cpu%d: starting\n", cpunum());
  kprintf("cpu%d: starting\n", cpunum());
  idt_init();       // load idt register

  static struct cpu *c asm("%gs:0");
  xchg(&c->started, 1); // tell startothers() we're up

}

static void
mpenter(void)
{
  seginit();
  lapic_init();
  mpmain();
}

void entryother_start();
void entryother_end();

struct segdesc mp_gdt[3] = {
  SEG(0,0,0,0),
  SEG(STA_X|STA_R, 0, 0xffffffff, 0),
  SEG(STA_W, 0, 0xffffffff, 0)
};

void
startothers(void)
{
  uchar *code;
  struct cpu *c;
  char *stack;

  code = (uchar *)(0x7000);
  
  memmove(code, (void *)entryother_start, (uint)(entryother_end - entryother_start));

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;


    stack = (char *)(uint32_t)pgalloc();
    *(void**)(code-4) = stack + KSTACKSIZE;	// used as temp stack
    *(void**)(code-8) = mpenter;	// used as callback
    *(int**)(code-12) = (void *) kva2pa(entrypgdir);
    *(struct segdesc **)(code-16) = mp_gdt;
    lapicstartap(c->apicid, kva2pa(code));

    // wait for cpu to finish mpmain()
    while(c->started == 0)
      ;
  }
}
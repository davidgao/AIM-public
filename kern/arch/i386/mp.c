#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <aim/console.h>
#include <aim/pmm.h>
#include <proc.h>
#include <asm.h>
#include <segment.h>
#include <arch-init.h>
#include <arch-mmu.h>
#include <mp.h>
#include <libc/string.h>


struct cpu cpus[NCPU];
int ismp;
int ncpu;
uchar ioapicid;

volatile uint32_t *lapic;

// MP init 

static uchar
sum(uchar *addr, int len)
{
  int i, sum;

  sum = 0;
  for(i=0; i<len; i++)
    sum += addr[i];
  return sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp*
mpsearch1(uint a, int len)
{
  uchar *e, *p, *addr;

  addr = P2V(a);
  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct mp))
    if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
      return (struct mp*)p;
  return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp*
mpsearch(void)
{
  uchar *bda;
  uint p;
  struct mp *mp;

  bda = (uchar *) P2V(0x400);
  if((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
    if((mp = mpsearch1(p, 1024)))
      return mp;
  } else {
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mpsearch1(p-1024, 1024)))
      return mp;
  }
  return mpsearch1(0xF0000, 0x10000);
}

// Search for an MP configuration table.  For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
static struct mpconf*
mpconfig(struct mp **pmp)
{
  struct mpconf *conf;
  struct mp *mp;

  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
    return 0;
  conf = (struct mpconf*) P2V((uint) mp->physaddr);
  if(memcmp(conf, "PCMP", 4) != 0)
    return 0;
  if(conf->version != 1 && conf->version != 4)
    return 0;
  if(sum((uchar*)conf, conf->length) != 0)
    return 0;
  *pmp = mp;
  return conf;
}

void
mpinit(void)
{
  uchar *p, *e;
  struct mp *mp;
  struct mpconf *conf;
  struct mpproc *proc;
  struct mpioapic *ioapic;

  if((conf = mpconfig(&mp)) == 0)
    return;
  ismp = 1;
  lapic = (uint*)conf->lapicaddr;
  for(p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; ){
    switch(*p){
    case MPPROC:
      proc = (struct mpproc*)p;
      if(ncpu < NCPU) {
        cpus[ncpu].apicid = proc->apicid;  // apicid may differ from ncpu
        ncpu++;
      }
      p += sizeof(struct mpproc);
      continue;
    case MPIOAPIC:
      ioapic = (struct mpioapic*)p;
      ioapicid = ioapic->apicno;
      p += sizeof(struct mpioapic);
      continue;
    case MPBUS:
    case MPIOINTR:
    case MPLINTR:
      p += 8;
      continue;
    default:
      ismp = 0;
      break;
    }
  }
  if(!ismp){
    // Didn't like what we found; fall back to no MP.
    ncpu = 1;
    lapic = 0;
    ioapicid = 0;
    return;
  }

  if(mp->imcrp){
    // Bochs doesn't support IMCR, so this doesn't run on Bochs.
    // But it would on real hardware.
    outb(0x22, 0x70);   // Select IMCR
    outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
  }
}


// MP booting 

void
seginit(void)
{
  struct cpu *c;

  // Map "logical" addresses to virtual addresses using identity map.
  // Cannot share a CODE descriptor for both kernel and user
  // because it would have to have DPL_USR, but the CPU forbids
  // an interrupt from CPL=0 to DPL=3.
  c = &cpus[cpunum()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);

  // Map cpu and proc -- these are private per cpu.
  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 8, 0);

  lgdt(c->gdt, sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);

  // Initialize cpu-local storage.
  // Note: cpu is defined in proc.h as %gs
  // cpu = c;
  set_gs_cpu(c);
  // proc = 0;
  set_gs_proc(NULL);
}

// Common CPU setup code.
void
mpmain(void)
{
  //cprintf("cpu%d: starting\n", cpunum());
  kprintf("cpu%d: starting\n", cpunum());
  idt_init();       // load idt register

  panic("This cpu is on!");

  struct cpu *c = get_gs_cpu();
  xchg(&c->started, 1);

  //TODO: scheduler();     // start running processes
}

// Other CPUs jump here from entryother.S.
static void
mpenter(void)
{
  //TODO: switchkvm();
  seginit();
  lapic_init();
  mpmain();
}

// extern pde_t entrypgdir[];  // For entry.S
void entryother_start();  // found in entryothers.S
void entryother_end();

struct segdesc mp_gdt[3] = {
  SEG(0,0,0,0),
  SEG(STA_X|STA_R, 0, 0xffffffff, 0),
  SEG(STA_W, 0, 0xffffffff, 0)
};

void
startothers(void)
{
  // extern uchar _binary_entryother_start[], _binary_entryother_size[];
  uchar *code;
  struct cpu *c;
  char *stack;

  // Write entry code to unused memory at 0x7000.
  // The linker has placed the image of entryother.S in
  // _binary_entryother_start.
  code = (uchar *)(0x7000);
  

  //memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);
  memmove(code, (void *)entryother_start, (uint)(entryother_end - entryother_start));

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell entryother.S what stack to use, where to enter, and what
    // pgdir to use. We cannot use kpgdir yet, because the AP processor
    // is running in low  memory, so we use entrypgdir for the APs too.
    // stack = (char *)kalloc();
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


// %GS util functions

void set_gs_cpu(struct cpu *temp) {
  __asm__ __volatile__(
    "mov %0, %%eax;"
    "mov %%eax, %%gs:0"
    ::"m"(temp)
  );
}

struct cpu *get_gs_cpu() {
  struct cpu *temp;
  __asm__ __volatile__(
    "mov %%gs:0, %%eax;"
    "mov %%eax, %0"
    :"=m"(temp)
  );
  return temp;
}
void set_gs_proc(struct proc *temp) {
  __asm__ __volatile__(
  "mov %0, %%eax;"
  "mov %%eax, %%gs:4"
  ::"m"(temp));
}
struct proc *get_gs_proc() {
  struct proc *temp;
  __asm__ __volatile__(
    "mov %%gs:0, %%eax;"
    "mov %%eax, %0"
    :"=m"(temp)
  );
  return temp;
}
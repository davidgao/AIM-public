#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "sys/types.h"
#include "aim/mmu.h"
#include "aim/spinlock.h"
#include "aim/panic.h"

void freerange(void *vstart, void *vend);
void kfree(addr_t *v);
// lds defines addr_t *__end;


struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  addr_t *p;
  p = (addr_t*)PGROUNDUP((uint32_t)vstart);
  for(; p + PGSIZE <= (addr_t*)vend; p += PGSIZE)
    kfree(p);
}

void
kfree(addr_t *v)
{
  struct run *r;

  if((uint32_t)v % PGSIZE || v < __end || premap_addr(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

addr_t*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  return (addr_t*)r;
}
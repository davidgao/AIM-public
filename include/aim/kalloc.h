
#ifndef __KALLOC_H_
#define __KALLOC_H_

#ifndef __ASSEMBLER__

void kinit1(void *vstart, void *vend);

void kinit2(void *vstart, void *vend);

void freerange(void *vstart, void *vend);

void kfree(addr_t *v);

addr_t* kalloc(void);

#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <aim/early_kmmap.h>
#include <aim/mmu.h>
#include <aim/panic.h>
#include <aim/pmm.h>
#include <libc/string.h>

addr_t* kalloc(void);

#define PGSIZE_EXT (PGSIZE<<10)

// Map 4M pages with specified paddr and vaddr
int page_index_early_map(pgindex_t *boot_page_index, addr_t paddr,
	void *vaddr, size_t size) {
    
    void *va = (void *)PGROUNDDOWN((uint32_t)vaddr);
    void *end = (void *)PGROUNDUP((uint32_t)vaddr + size - 1);
    paddr = PGROUNDDOWN(paddr);

    pte_t *pte;
    for(; va <= end; va += PGSIZE_EXT) {
        pte = (pte_t *)&boot_page_index[PDX(va)];
        *pte = (uint32_t)(paddr | PTE_P | PTE_W | PTE_PS);
        paddr += PGSIZE_EXT;
    }
    return 1;
}

// Set up linear mapping for early mapping
void early_mm_init(void) {
    page_index_early_map(entrypgdir, (addr_t)0, 
        (void *)KERN_BASE, PHYSTOP);
    page_index_early_map(entrypgdir, (addr_t)0, 
        (void *)0, PHYSTOP);
    page_index_early_map(entrypgdir, (addr_t)PHYSTOP, 
        (void *)0xfe000000, 0x1000000);
}

// check validity of PDE
bool early_mapping_valid(struct early_mapping *entry)
{
	return true;
}

// load boot_page_index as %%cr3
void mmu_init(pgindex_t *boot_page_index)
{
    __asm__ __volatile__ (
        "movl   %0, %%eax;"
        "movl   %%eax, %%cr3"
        ::"m"(boot_page_index)
        
    );
}

void page_index_clear(pgindex_t *boot_page_index) {
    memset(boot_page_index, JUNKBYTE, PGSIZE); 
}

// Get or alloc a page table in given pagedir 
static pte_t* walk_page_dir(pgindex_t *pgindex, vaddr_t *vaddr, int alloc) {
    
    pde_t *pde = (pde_t *)&pgindex[PDX(vaddr)];
    pte_t *pt;
    
    if(*pde & PTE_P){
            pt = (pte_t*)postmap_addr(PTE_ADDR(*pde));  //PTE_ADDR + KERN_BASE
    } else {
        if(!alloc || (pt = (pte_t *)(uint32_t)pgalloc()) == 0)
            return 0;
        memset(pt, 0, PGSIZE);  // static inline in arch-mmu.h
        *pde = premap_addr(pt) | PTE_P | PTE_W | PTE_U;
    }
    return &pt[PTX(vaddr)];

    
}

/* Map virtual address starting at @vaddr to physical pages at @paddr, with
 * VMA flags @flags (VMA_READ, etc.) Additional flags apply as in kmmap.h.
 */
int map_pages
    (
    pgindex_t *pgindex, void *vaddr, addr_t paddr, size_t size,
    uint32_t flags
    ) 
{
    //TODO: Assume similar function with xv6 mappages
    
    vaddr_t *va = (vaddr_t *)PGROUNDDOWN((uint32_t)vaddr);
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)(vaddr + size - 1)));
    
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 1)) == 0) 
            return -1;  // fail to get page table entry
        if(*pte & PTE_P)
            panic("remap in map_pages");
        *pte = PTE_ADDR(paddr) | PTE_FLAGS(flags) | PTE_P; 
        paddr += PGSIZE;
        
    }
    //TODO: VMA_READ?! and other flags?

    return end + PGSIZE - 1 - va;
}

/*
 * Unmap but do not free the physical frames
 * Returns the size of unmapped physical memory (may not equal to @size), or
 * negative for error.
 * The physical address of unmapped pages are stored in @paddr.
 */
ssize_t unmap_pages(pgindex_t *pgindex, void *vaddr, size_t size, addr_t *paddr)
{
    //TODO: implment
    vaddr_t *va = (vaddr_t *)PGROUNDDOWN((uint32_t)vaddr);
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)vaddr) + size - 1);
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 0)) == 0) // alloc not allowed
            continue;  // continue when fail to get pte !
        
        *pte = 0;     
    }
    return end - va + PGSIZE - 1;
}
/*
 * Change the page permission flags without changing the actual mapping.
 */
int set_pages_perm(pgindex_t *pgindex, void *vaddr, size_t size, uint32_t flags)
{
    
    vaddr_t *va = (vaddr_t *)PGROUNDDOWN((uint32_t)vaddr);
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)vaddr) + size - 1);
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 0)) == 0) // alloc not allowed
            return -1;  // fail to get page dir
        
        *pte = (PTE_ADDR(*pte) | PTE_FLAGS(flags)) ^ PTE_FLAGS(PTE_U);        
    }
    return end - va + PGSIZE - 1;
    
}
/*
 * Invalidate pages, but without actually deleting the page index entries, 
 * if possible.
 */
ssize_t invalidate_pages(pgindex_t *pgindex, void *vaddr, size_t size,
    addr_t *paddr) 
{
    vaddr_t *va = (vaddr_t *)PGROUNDDOWN((uint32_t)vaddr);
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)vaddr) + size - 1);
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 0)) == 0) // alloc not allowed
            continue;  // continue when fail to get pte !
        
        *pte = PTE_ADDR(*pte) | 0;        
    }
    return end - va + PGSIZE - 1;
}
/* Switch page index to the given one */
int switch_pgindex(pgindex_t *pgindex) {
    mmu_init(pgindex);
    return 1;
}
/* Get the currently loaded page index structure */
pgindex_t *get_pgindex(void){
    pgindex_t *ret;
    __asm__ __volatile__ (
        "mov %%cr3, %0;"
        :"=a"(ret)
    );
    return ret;
}
/* Trace a page index to convert from user address to kernel address */
void *uva2kva(pgindex_t *pgindex, void *uaddr){
    //TODO: check
    vaddr_t *a = (vaddr_t *)PTE_ADDR(walk_page_dir(pgindex, uaddr, 0));
    if(a == 0)
        return 0;
    return (void *)postmap_addr((uint32_t)a | ((uint32_t)(uaddr) & 0xfff));
}
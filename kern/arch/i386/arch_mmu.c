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
#include <aim/early_kmmap.h>
#include <aim/mmu.h>
#include <aim/panic.h>

addr_t* kalloc(void);

// Clear low-addr mapping to disable user space while kernel is high
void page_index_clear(pgindex_t *boot_page_index) {
    //TODO: write invalid pagedir to?
    uint32_t n = KERN_BASE / (4<<20);   // 4M page 
    memset(entrypgdir, 0, n << 2);   
}

// Map 4M pages with specified paddr and vaddr
int page_index_early_map(pgindex_t *boot_page_index, addr_t paddr,
	void *vaddr, size_t size) {
    
    vaddr_t *va = (vaddr_t *)vaddr;
    vaddr_t *end = (vaddr_t *)(vaddr + size);
    pte_t *pte;
    for(; va < end; va += PGSIZE) {
        pte = (pte_t *)&boot_page_index[PDX(va)];
        *pte = (uint32_t)(paddr | PTE_P | PTE_W | PTE_PS);
        paddr += PGSIZE;
    }
    return end - va;
}

// Set up linear mapping for early mapping
void early_mm_init(void) {
    // user space usage not allowed, only kernel is mapped
    page_index_early_map(entrypgdir, (addr_t)0, (void *)KERN_BASE, PHYSTOP - 0);
    
    // invalidate low addr pages (user space)
    page_index_clear(entrypgdir);
}

/***************************************************************/
// Get or alloc a page table in given pagedir 
static pte_t* walk_page_dir(pgindex_t *pgindex, vaddr_t *vaddr, int alloc) {
    
    pde_t *pde = (pde_t *)&pgindex[PDX(vaddr)];
    pte_t *pt;
    
    if(*pde & PTE_P){
            pt = (pte_t*)postmap_addr(PTE_ADDR(*pde));  //PTE_ADDR + KERN_BASE
    } else {
        if(!alloc || (pt = (pte_t*)kalloc()) == 0)
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
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)vaddr) + size - 1);
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 1)) == 0) 
            return -1;  // fail to get page dir
        if(*pte & PTE_P)
            panic("remap in map_pages");
        *pte = paddr | PTE_FLAGS(flags) | PTE_P;    //TODO: why P?
        paddr += PGSIZE;
        
    }
    //TODO: VMA_READ?! and other flags?

    return end - va + PGSIZE - 1;
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
    return 0;
}
/*
 * Change the page permission flags without changing the actual mapping.
 */
int set_pages_perm(pgindex_t *pgindex, void *vaddr, size_t size, uint32_t flags)
{
    //TODO: check
    
    vaddr_t *va = (vaddr_t *)PGROUNDDOWN((uint32_t)vaddr);
    vaddr_t *end = (vaddr_t *)(PGROUNDDOWN((uint32_t)vaddr) + size - 1);
    pte_t *pte;
    for(; va <= end; va += PGSIZE) {
        if((pte = walk_page_dir(pgindex, va, 0)) == 0) // alloc not allowed
            return -1;  // fail to get page dir
        
        *pte = PTE_ADDR(*pte) | PTE_FLAGS(flags);        
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
    //TODO: implement
    return 0;
}
/* Switch page index to the given one */
int switch_pgindex(pgindex_t *pgindex) {
    //TODO: implement
    return 0;
}
/* Get the currently loaded page index structure */
pgindex_t *get_pgindex(void){
    //TODO: implement
    return 0;
}
/* Trace a page index to convert from user address to kernel address */
void *uva2kva(pgindex_t *pgindex, void *uaddr){
    //TODO: check
    vaddr_t *a = (vaddr_t *)PTE_ADDR(walk_page_dir(pgindex, uaddr, 0));
    if(a == 0)
        return 0;
    return (void *)postmap_addr((uint32_t)a | ((uint32_t)(uaddr) & 0xfff));
}



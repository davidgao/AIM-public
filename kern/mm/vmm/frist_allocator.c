#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <aim/vmm.h>
#include <aim/pmm.h>
#include <aim/panic.h>
#include <aim/mmu.h>
#include <aim/console.h>

/* This file is for simple allocator */

#define BLOCK_SIZE 0x8
#define BLOCK_MASK (BLOCK_SIZE - 1)
#define BLOCK_ROUNDUP(x) (((x) + BLOCK_SIZE - 1) & ~BLOCK_MASK)
        
struct early_header {
    uint16_t size;      // free space left
    void *start;        // start of free space
    void *head;         // start of whole space
    bool initialized;
} ;

struct early_list {
    struct early_list *pre;
    struct early_list *next;
} freelist;

static struct early_header temp_eh;

static void e_list_add(struct early_list *new, struct early_list *head) {
    head->next->pre = new;
    new->next = head->next;
    new->pre = head;
    head->next = new;
}

static void e_list_del(struct early_list *head) {
    head->pre->next = head->next;
    head->next->pre = head->pre;
}

static bool e_list_empty(struct early_list *head) {
    return head->next == head;
}

static struct early_list *e_list_get(struct early_list *head) {
    struct early_list *ret = head->next;
    e_list_del(ret);
    return ret;
}

// continous stack space for early simple allocator
void early_simple_init(struct early_header *eh, 
    void *start, uint32_t size) 
{
    eh->start = eh->head = start;
    eh->size = size;
    eh->initialized = true;

    freelist.pre = &freelist;
    freelist.next = &freelist;

    for (int i=0; i<size; i+=sizeof(struct page_node)) {
        e_list_add(start+i, &freelist);
    }
}

void *early_simple_alloc(size_t size, gfp_t flags) {
    if(e_list_empty(&freelist))
        return NULL;
    temp_eh.size -= size;
    return (void *)e_list_get( &freelist);
}

void early_simple_free(void *obj) {
    temp_eh.size += sizeof(struct early_list);
    e_list_add(obj, &freelist);
}

static size_t sizeof_list(void *obj) {
    return sizeof(struct early_list);
}

void sleep1();
void page_alloc_init(addr_t start, addr_t end);

static struct simple_allocator temp_simple_allocator = {
	.alloc	= early_simple_alloc,
	.free	= early_simple_free,
    .size	= sizeof_list
};


void master_early_simple_alloc(void *start, void *end) {
    early_simple_init(&temp_eh, start, end - start);
    set_simple_allocator(&temp_simple_allocator);
}

void *get_early_end() {
    return temp_eh.start;
}
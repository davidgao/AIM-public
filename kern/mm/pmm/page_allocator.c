#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <aim/pmm.h>
#include <aim/mmu.h>
#include <libc/string.h>
#include <aim/vmm.h>
#include <aim/panic.h>
#include <aim/console.h>

/* Designed for allocate consecutive pages */

// 4M block at most
#define NLEVEL 11
#define MAX_BLOCK ((PGSIZE)<<((NLEVEL) - 1))
#define PAGE_FRAME(x) ((x)>>12)
#define BIT_COUNT(order, paddr) (PAGE_FRAME((paddr)) >> ((order)+1))
#define BLOCK_ALIGN(order, paddr) ((paddr) & (~((PGSIZE<<(order)) - 1)))

// Bitmap
typedef uint8_t bitmap;     // uint8_t[ 7 ~ 0 ]
static bitmap *page_map[NLEVEL];
// static int ntop_level_pages = 0;
#define MAP_SIZE(x) ((x + 7)>>3)

static struct page_node pool[NLEVEL];

// Statistics
static uint64_t global_empty_pages = 0;

/*************** Data structure interface ***************************/
static uint8_t read_map_bitcount(int order, int bitcount) {
    if(order >= NLEVEL - 1 || page_map[order] == NULL) { 
        panic("read_map illeagal status");
    }
    bitmap *map = page_map[order];
    uint8_t temp = map[bitcount >> 3]; 
    temp = (temp >> (bitcount & 0x7)) & 0x1;
    return temp;
}

static void switch_map_bitcount(int order, int bitcount) {
    if(order >= NLEVEL - 1 || page_map[order] == NULL) { 
        panic("read_map illeagal status");
    }
    bitmap *map = page_map[order];
    uint8_t mask = 1 << (bitcount & 0x7);
    map[bitcount >> 3] ^= mask;
}

static void set_map_bitcount(int order, int bitcount, uint8_t bit) { 
    if(order >= NLEVEL - 1 || page_map[order] == NULL) { 
        panic("read_map illeagal status");
    }
    bitmap *map = page_map[order];
    uint8_t mask = 1 << (bitcount & 0x7);
    bit &= 0x1;
    map[bitcount >> 3] = (~mask & map[bitcount >> 3]) 
        | (bit << (bitcount & 0x7));
}

void *early_simple_alloc(size_t size, gfp_t flags) ;
void early_simple_free(void *obj) ;
static struct page_node *new_page_node() {
    struct page_node *temp = 
        (struct page_node *)early_simple_alloc(sizeof(struct page_node), GFP_ZERO);
    if(temp != NULL)
        memset(temp, 0, sizeof(struct page_node));
    else
        panic("new_page_node fail to alloc");
    temp->pre = temp;
    temp->next = temp;
    return temp;
}

static void delete_page_node(struct page_node *node) {
    early_simple_free(node);
} 

static void list_add(struct page_node *new, struct page_node *head) {
    head->next->pre = new;
    new->next = head->next;
    new->pre = head;
    head->next = new;
} 

static void list_del(struct page_node *head) {
    head->pre->next = head->next;
    head->next->pre = head->pre;
}

static bool list_empty(struct page_node *head) {
    return head->next == head;
}

static struct page_node *add_pool(int order, addr_t paddr) {

    struct page_node *temp = new_page_node();

    temp->paddr = paddr;
    list_add(temp, &pool[order]);
    return temp;
}

static addr_t from_pool(int order) {
    addr_t ret;
    if(list_empty(&pool[order])) {
        return EOF;
    }
    else {
        struct page_node *temp = pool[order].next;
        ret = temp->paddr; // head should not be used
        list_del(temp);
        delete_page_node(temp);
    }
    return ret;
}

static struct page_node *search_pool(int order, addr_t paddr) {
    struct page_node *p = &pool[order];
    bool found = false;
    // kprintf("\nsearch_pool: looking for %p\n", paddr);
    do {
        p = p->next;
        // kprintf("%p ", p->paddr);
        if(p->paddr == paddr) {
            found = true;
            break;
        }
    } while(p->next!=&pool[order]);

    if(found) 
        return p;
    else 
        return NULL;
}

static void remove_node(struct page_node *node) {
    list_del(node);
    delete_page_node(node);
}

/*************** Inner Util Functions ***************************/

static addr_t page_init_range(addr_t start, addr_t end, uint8_t order) {
    // order in [0, NLEVEL-1]
    start = PGROUNDDOWN(start);
    end = PGROUNDDOWN(end);
    size_t size = (PGSIZE) << order;
    while((end - start) > 2 * size) {
        // free [start, start + size) and one more to make a pair
        add_pool(order, start);
        start += size;
        add_pool(order, start);
        start += size;
    }

    return start;   // start less than or equal to end
}

static addr_t split_page_node(int order) {
    // split (order, top]

    int top = order; 
    addr_t start;
    while(top < NLEVEL) {
        start = from_pool(top);
        if(start != EOF)
            break;
        top ++;
    }
    // kprintf("split_page_node: top is %d\n", top);
    if(top >= NLEVEL || top == order) {
        return EOF;
    }
    // kprintf("start is %p", start);
    // split from top to order (loop recursive)
    for(int i=top; i>order; --i) {
        // split order i to get (i-1)

        // order i block is used but don't set highest level
        if(i != NLEVEL - 1)
            switch_map_bitcount(i, BIT_COUNT(i, start));
        // add left child in pool
        add_pool(i-1, start + (PGSIZE<<(i-1)));
        // lower level is sure to be 1
        set_map_bitcount(i-1, BIT_COUNT(i-1, start), 0);

        // kprintf("split %d for %d: switched %d\n", i, i-1, BIT_COUNT(i, start));
        // kprintf("\tadd_pool(%x) set %d\n", start + (PGSIZE<<(i-1)), BIT_COUNT(i-1, start));
        
    }
    // kprintf("split_page_node return %p", start);
    return start;
}

static uint32_t merge_page_node(int order, addr_t paddr) {
    // assume paddr is already aligned
    uint8_t temp;
    struct page_node *node;
    addr_t p1, p2;
    // merge order to get order + 1
    while(order < NLEVEL - 1){
        temp = read_map_bitcount(order, BIT_COUNT(order, paddr));
        if(temp) {
            // should merge
            // recycle from free pool
            
            p1 = BLOCK_ALIGN(order + 1, paddr); // plus one to get left sib
            p2 = p1 + (PGSIZE << order);    // right sib
            
            // kprintf("merge_page_node: order is %d\n", order);
            // kprintf("merge_page_node: paddr=0x%p ", paddr);
            // kprintf("p1=%p", p1);
            // kprintf("p2=%p\n", p2);

            if(p1 == paddr) 
                node = search_pool(order, p2);
            else if(p2 == paddr)
                node = search_pool(order, p1);
            else {
                panic("merge_page_node: unexpected calculation");
            }
            
            if(node == NULL) {
                panic("merge_page_node: Illegal status missing pool node");
            }

            remove_node(node);

            switch_map_bitcount(order, BIT_COUNT(order, paddr));

            // prepare for next loop
            order ++;
            paddr = p1;

        }
        else {
            // 0 in bitmap means sib used
            break;
        } 
    }
    add_pool(order, paddr);
    return (1 << order);
}

/*************** Interfaces and bundle parts ***************************/
static uint8_t page_map_buf[10240];
// Manage PADDR
void page_alloc_init(addr_t start, addr_t end) {
    // Initialize

    for(int i=0; i<NLEVEL; ++i) {
        pool[i].paddr = EOF;
        pool[i].pre = &pool[i];
        pool[i].next = &pool[i];
    }

    // Round addr conservatively
    // start = PGROUNDUP(start);
    // end = PGROUNDDOWN(end);
    #define ROUNDUP_4M(paddr) ((paddr + (1<<22) - 1) & ~((1<<22) - 1))
    #define ROUNDDOWN_4M(paddr) ((paddr) & ~((1<<22) - 1))
    start = ROUNDUP_4M(start);
    end = ROUNDDOWN_4M(end);
    #undef ROUNDUP_4M
    #undef ROUNDDOWN_4M

    end = page_init_range(start, end, NLEVEL - 1); 

    global_empty_pages = (end - start) / PGSIZE;
    
    void *early_simple_alloc(size_t size, gfp_t flags);

    bitmap *space = (bitmap *)page_map_buf;
    memset(space, 0, global_empty_pages >> 3);
    int temp = global_empty_pages>>4;   // 0 order map size
    
    for(int i=0; i<NLEVEL - 1; ++i) {
        page_map[i] = space;
        space += temp;
        temp >>= 1;
    }
}

int bundle_pages_alloc(struct pages *pages) {
    int n = (pages->size + PGSIZE - 1) / PGSIZE;
    int npages = 0x1, order = 0;
    while(npages < n) {
        npages <<= 1;
        order ++;
    }
    if(order >= NLEVEL)
        return EOF;
    addr_t paddr = from_pool(order);
    if(EOF == paddr) {
        // need split
        paddr = split_page_node(order);  // split (order, top]
        if(paddr == EOF)
            return EOF;
        // split set bitmap 0
    }


    switch_map_bitcount(order, BIT_COUNT(order, paddr));
    
    pages->paddr = paddr;
    global_empty_pages -= npages;

    // kprintf("get paddr %p\n", pages->paddr);
    return 0;
}

void bundle_pages_free(struct pages *pages) {

    // assume [paddr, paddr+size) is inside proper block
    // TODO: and has no subblock? 
    int n = (pages->size + PGSIZE - 1) / PGSIZE;
    int npages = 0x1, order = 0;
    while(npages < n) {
        npages <<= 1;
        order ++;
    }
    global_empty_pages += npages;
    
    if(order >= NLEVEL)
        panic("bundle_pages_free: too large order");

    addr_t start = BLOCK_ALIGN(order, pages->paddr);
    npages = merge_page_node(order, start);    

}

addr_t bundle_pages_size() {
    return global_empty_pages * PGSIZE;
}
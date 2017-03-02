#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <list.h>

#include <mm.h>
#include <pmm.h>
#include <vmm.h>
#include <panic.h>

#define ALLOC_ALIGN 16

struct blockhdr {
	size_t size;
	bool free;
	gfp_t flags;
	struct list_head node;
	char __padding[12];
};

#define PAYLOAD(bh)		((void *)((struct blockhdr *)(bh) + 1))
#define HEADER(payload)		((struct blockhdr *)(payload) - 1)

static struct list_head __bootstrap_head;
static struct list_head __head;

static inline void *__alloc(struct list_head *head, size_t size, gfp_t flags)
{
	struct blockhdr *this;
	size_t allocsize, newsize;

	size = ALIGN_ABOVE(size, ALLOC_ALIGN);
	allocsize = size + sizeof(struct blockhdr);

	for_each_entry(this, head, node) {
		if (this->size >= allocsize) { break; }
	}

	if (&this->node == head) {
		struct pages pages = {
			.paddr = 0,
			.size = ALIGN_ABOVE(size, PAGE_SIZE),
			.flags = flags
		};
		struct blockhdr *tmp;
		if (alloc_pages(&pages) == EOF) {
			return NULL;
		}

		this = (struct blockhdr *)pa2kva((size_t)pages.paddr);
		this->size = pages.size;
		this->free = true;
		this->flags = flags;

		for_each_entry(tmp, head, node) {
			if (tmp >= this) { break; }
		}
		list_add_before(&this->node, &tmp->node);
	}

	newsize = this->size - allocsize;
	if (newsize >= sizeof(struct blockhdr) + ALLOC_ALIGN) {
		struct blockhdr *newblock = ((void *)this) + allocsize;
		newblock->size = newsize;
		newblock->free = true;
		newblock->flags = this->flags;
		this->size = allocsize;
		list_add_after(&newblock->node, &this->node);
	}
	this->free = false;
	list_del(&this->node);
	return PAYLOAD(this);
}

static inline void __free(struct list_head *head, void *obj)
{
	struct blockhdr *this, *tmp;

	this = HEADER(obj);
	this->free = true;

	for_each_entry(tmp, head, node) {
		if (tmp >= this) { break; }
	}
	list_add_before(&this->node, &tmp->node);

	tmp = list_entry(this->node.prev, struct blockhdr, node);
	if (
		&tmp->node != head && 
		(void *)tmp + tmp->size == (void *)this 
	) {
		tmp->size += this->size;
		list_del(&this->node);
		this = tmp;
	}

	tmp = list_entry(this->node.next, struct blockhdr, node);
	if (
		&tmp->node != head && 
		(void *)this + this->size == (void *)tmp 
	) {
		this->size += tmp->size;
		list_del(&tmp->node);
	}

	size_t end = (size_t)this + this->size;
	size_t first_border = ALIGN_ABOVE((size_t)this, PAGE_SIZE);
	size_t last_border = ALIGN_BELOW(end, PAGE_SIZE);
	if (first_border < last_border) {
		struct pages pages = {
			.paddr = (addr_t)first_border,
			.size = (addr_t)(last_border - first_border),
			.flags = this->flags
		};

		if (last_border < end) {
			tmp = (struct blockhdr *)last_border;
			tmp->size = end - last_border;
			tmp->free = true;
			tmp->flags = this->flags;
			list_add_after(&tmp->node, &this->node);
		}

		if (first_border > (size_t)this) {
			this->size = first_border - (size_t)this;
		} else {
			list_del(&this->node);
		}

		free_pages(&pages);
	}
}

static size_t __size(void *obj)
{
	struct blockhdr *this;

	this = HEADER(obj);

	return this->size - sizeof(struct blockhdr);
}

static void *__bootstrap_alloc(size_t size, gfp_t flags)
{
	return __alloc(&__bootstrap_head, size, flags);
}

static void *__proper_alloc(size_t size, gfp_t flags)
{
	return __alloc(&__head, size, flags);
}

static void __bootstrap_free(void *obj)
{
	__free(&__bootstrap_head, obj);
}

static void __proper_free(void *obj)
{
	__free(&__head, obj);
}

int simple_allocator_bootstrap(void *pt, size_t size)
{
	struct blockhdr *block = pt;
	block->size = size;
	block->free = true;
	list_init(&__bootstrap_head);
	list_add_after(&block->node, &__bootstrap_head);

	struct simple_allocator allocator = {
		.alloc	= __bootstrap_alloc,
		.free	= __bootstrap_free,
		.size	= __size
	};
	set_simple_allocator(&allocator);
	return 0;
}

int simple_allocator_init(void)
{
	list_init(&__head);

	struct simple_allocator allocator = {
		.alloc	= __proper_alloc,
		.free	= __proper_free,
		.size	= __size
	};
	set_simple_allocator(&allocator);
	return 0;
}
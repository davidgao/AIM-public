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

#ifndef _AIM_VMM_H
#define _AIM_VMM_H

#include <aim/gfp.h>

/*
 * Two kinds of memory object allocators (may) exist inside a running kernel.
 * - One or two simple allocators: used in kmalloc()-like routines.
 *   These access interfaces do not allow multiple algorithms, nor multiple
 *   instances inside a running kernel. The only case with two allocators
 *   is the bootstrap of memory management subsystem, where we have to prepare
 *   one temporary allocator to start the page allocator. The temporary
 *   instance is destroyed in the process and is never visible to other
 *   subsystems.
 * - Zero to many cached allocators: These allocators allocate initialized
 *   objects, and preserve their states across multiple allocations. They are
 *   here to prevent the need of frequent initialization of complex objects.
 *   SLAB algorithm is popular for this use.
 */

#ifndef __ASSEMBLER__

/* currently ignored */

struct simple_allocator {
	void *(*alloc)(size_t size, gfp_t flags);
	void (*free)(void *obj);
	size_t (*size)(void *obj);
};

int simple_allocator_bootstrap(void *pt, size_t size);
int simple_allocator_init(void);
void set_simple_allocator(struct simple_allocator *allocator);
void get_simple_allocator(struct simple_allocator *allocator);
/* The above get* and set* functions COPIES structs */

struct allocator_cache {
	//lock_t lock;
	void *head; /* recognized by the allocator */
	size_t size;
	size_t align;
	gfp_t flags;
	void (*create_obj)(void *obj);
	void (*destroy_obj)(void *obj);
};

struct caching_allocator {
	int (*create)(struct allocator_cache *cache);
	int (*destroy)(struct allocator_cache *cache);
	void *(*alloc)(struct allocator_cache *cache);
	int (*free)(struct allocator_cache *cache, void *obj);
	void (*trim)(struct allocator_cache *cache);
};

void set_caching_allocator(struct caching_allocator *allocator);

void *kmalloc(size_t size, gfp_t flags);
static inline void *kcalloc(size_t nmemb, size_t size, gfp_t flags)
{
	void *result = kmalloc(nmemb * size, flags);
	memset(result, 0, nmemb * size);
	return result;
}
void kfree(void *obj);
size_t ksize(void *obj);

int cache_create(struct allocator_cache *cache);
int cache_destroy(struct allocator_cache *cache);
void *cache_alloc(struct allocator_cache *cache);
int cache_free(struct allocator_cache *cache, void *obj);
void cache_trim(struct allocator_cache *cache);

#endif /* !__ASSEMBLER__ */

#endif /* _AIM_VMM_H */


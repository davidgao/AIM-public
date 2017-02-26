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

/*
 * This file provides an implementation for architecture-dependent
 * initialization to set up early mappings without worrying about
 * lower implementation and later kernel functionalities.
 * These routines MUST always use PC(or IP on some platforms)-relative
 * addressing to access its components, in order to behave as expected
 * across the initialization of MMU (or similar address translation
 * methods).
 */

#include <sys/types.h>
#include <aim/console.h>
#include <aim/early_kmmap.h>
#include <aim/debug.h>
#include <errno.h>
#include <util.h>

/* TODO move this to configuration */
#define EARLY_MAPPING_QUEUE_LENGTH	10

static struct early_mapping queue[EARLY_MAPPING_QUEUE_LENGTH];
static int queue_size;
static void *mem_top, *kmmap_top;

void early_mapping_clear(void)
{
	queue_size = 0;
	mem_top = (void *)KERN_BASE;
	kmmap_top = (void *)KMMAP_BASE;
	kpdebug("Early mappings cleared.\n");
}

int early_mapping_add(struct early_mapping *entry)
{
	/* Prevent damage in case of bad data structure. */
	assert(queue_size <= EARLY_MAPPING_QUEUE_LENGTH);

	/* Fail in case of full queue. */
	if (queue_size == EARLY_MAPPING_QUEUE_LENGTH)
		return -ENOMEM;
	/* Alignment check */
	if (!early_mapping_valid(entry))
		return -EINVAL;
	/* VADDR overlap check */
	for (int i = 0; i < queue_size; i += 1) {
		if (OVERLAP(
			entry->vaddr, entry->size,
			queue[i].vaddr, queue[i].size
		)) return -EINVAL;
	}
	/* Commit change */
	queue[queue_size++] = *entry;
	return 0;
}

size_t early_mapping_add_memory(addr_t base, size_t size)
{
	/* Check available address space */
	if ((size_t)mem_top >= KMMAP_BASE)
		return 0;
	if (size > KMMAP_BASE - (size_t)mem_top)
		size = KMMAP_BASE - (size_t)mem_top;
	/* Construct entry */
	struct early_mapping entry = {
		.paddr	= base,
		.vaddr	= mem_top,
		.size	= (size_t)size,
		.type	= EARLY_MAPPING_MEMORY
	};
	/* Apply */
	if (early_mapping_add(&entry) < 0)
		return 0;
	mem_top += size;
	return size;
}

void *early_mapping_add_kmmap(addr_t base, size_t size)
{
	/* Check available address space */
	if ((size_t)kmmap_top >= RESERVED_BASE)
		return NULL;
	if (size > RESERVED_BASE - (size_t)kmmap_top)
		return NULL;
	/* Construct entry */
	struct early_mapping entry = {
		.paddr	= base,
		.vaddr	= kmmap_top,
		.size	= size,
		.type	= EARLY_MAPPING_KMMAP
	};
	/* Apply */
	if (early_mapping_add(&entry) < 0)
		return NULL;
	kmmap_top += size;
	return (void *)entry.vaddr;
}

/*
 * basic iterator. Caller should not work with internal data structure.
 * If given a pointer to some early mapping entry, return the next one.
 * If given a NULL, return the first entry.
 * If given some invalid entry, or if no more entries are available, return
 * NULL.
 */
struct early_mapping *early_mapping_next(struct early_mapping *base)
{
	struct early_mapping *next;

	/* Pick first or next entry */
	if (base == NULL) {
		next = &queue[0];
	} else {
		next = base + 1; /* One entry */
	}
	/* Validate and return */
	int tmp = next - &queue[0];
	if (tmp < 0 || tmp >= queue_size)
		return NULL;
	else
		return next;
}


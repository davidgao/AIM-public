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

#ifndef _AIM_EARLY_KMMAP_H
#define _AIM_EARLY_KMMAP_H

#ifndef __ASSEMBLER__

struct early_mapping {
	addr_t	paddr;
	void	*vaddr;
	size_t	size;
	int	type;
#define	EARLY_MAPPING_MEMORY	0	/* linear memory mappings */
#define EARLY_MAPPING_KMMAP	1	/* kmmap allocated entries */
#define EARLY_MAPPING_TEMP	2	/* discarded */
#define EARLY_MAPPING_OTHER	3	/* kmmap explicit entries */
};

/* Architecture-specific */
bool early_mapping_valid(struct early_mapping *entry);

/* Architecture-independent */
void early_mapping_clear(void);
size_t early_mapping_add_memory(addr_t base, size_t size);
void *early_mapping_add_kmmap(addr_t base, size_t size);
int early_mapping_add(struct early_mapping *entry);
struct early_mapping *early_mapping_next(struct early_mapping *base);

#endif /* !__ASSEMBLER__ */

#endif /* _AIM_EARLY_KMMAP_H */


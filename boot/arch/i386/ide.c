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
#include <aim/boot.h>
#include <aim/io.h>
#include <asm.h>

static inline
void waitdisk(void)
{
	while ((in8(0x1F7) & 0xC0) != 0x40);
}

void readsect(void *dst, off_t offset)
{
	waitdisk();
	out8(0x1F2, 1);
	out8(0x1F3, (uint8_t)offset);
	out8(0x1F4, (uint8_t)(offset >> 8));
	out8(0x1F5, (uint8_t)(offset >> 16));
	out8(0x1F6, (uint8_t)(offset >> 24) | 0xE0);
	out8(0x1F7, 0x20);

	waitdisk();
	insl(0x1F0, dst, SECT_SIZE / sizeof(uint32_t));
}


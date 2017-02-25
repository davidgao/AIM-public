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
#include <elf.h>
#include <asm.h>

#define SECT_SIZE	512

elf_hdr * ELFHDR = (elf_phdr *)0x10000;

static void waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40);
}

static void readsect(void *dst, uint32_t secno) {
    waitdisk();

    outb(0x1F2, 1);
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);

    waitdisk();

    insl(0x1F0, dst, SECT_SIZE);
}

static void readseg(uintptr_t va, uint32_t count, uint32_t offset) {
    uintptr_t end_va = va + count;

    va -= offset % SECT_SIZE;

    uint32_t secno = (offset / SECT_SIZE) + 1;

    for (; va < end_va; va += SECT_SIZE, secno ++) {
        readsect((void *)va, secno);
    }
}

__noreturn
void bootmain(void)
{
	readseg((uintptr_t)ELFHDR, SECT_SIZE * 8, 0);

    elf_phdr *ph, *eph;

    ph = (elf_phdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph ++) {
        readseg((uintptr_t) (ph->p_paddr & 0xFFFFFF), ph->p_memsz, ph->p_offset);
    }

	void (*kernel_entry)(void) = (void (*)(void))(ELFHDR->e_entry);
    kernel_entry();

bad:
	while (1);
}


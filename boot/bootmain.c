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
#include <elf.h>
#include <asm.h>

#define ELF_BUF_SIZE	4096

static uint8_t elf_hdr_buf[ELF_BUF_SIZE];

void readseg(void *pa, size_t count, off_t offset)
{
	void *epa = pa + count;
	// Round down to sector boundary.
	pa -= offset % SECT_SIZE;
	// Translate from bytes to sectors; kernel starts at sector 1.
	offset = offset / SECT_SIZE;
	// we load in increasing order.
	while (pa < epa) {
		readsect(pa, offset);
		pa += SECT_SIZE;
		offset++;
	}
}

#pragma GCC diagnostic ignored "-Wunused-parameter" 	/* remove after impl */
static inline
bool is_elf(const elf_hdr *elf)
{
	/*static const uint8_t magic[] = {
		0x7f, 0x45, 0x4c, 0x46,
		0x02, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	for (int i = 0; i < (int)sizeof(magic); i++) {
		if (elf->e_ident[i] != magic[i]) return false;
	}*/
	return true;
}
#pragma GCC diagnostic pop				/* remove after impl */

static inline
uint32_t get_lba(int n) // 1-4
{
	uint32_t lba;
	for (int i = 0; i < 4; i++) {
		((uint8_t *)&lba)[i] = mbr[446 + 16 * (n - 1) + 8 + i];
	}
	return lba;
}

__noreturn
void bootmain(void)
{
	elf_hdr *elf = (elf_hdr *)&elf_hdr_buf[0];
	elf_phdr *ph, *eph;
	uint32_t base = get_lba(2) * SECT_SIZE; // TODO boot any partition
	void (*entry)(void);
	void *pa;

	readseg(&elf_hdr_buf, ELF_BUF_SIZE, base);
	// magic check
	if (!is_elf(elf)) goto fail;

	// Load each program segment
	ph = (elf_phdr *)((void *)elf + elf->e_phoff);
	eph = ph + elf->e_phnum;
	for (; ph < eph; ph++){
		/* load flag */
		if (ph->p_type != PT_LOAD) continue;
		pa = (void *)ph->p_paddr;
		readseg(pa, ph->p_filesz, base + ph->p_offset);
		if (ph->p_memsz > ph->p_filesz)
		stosb(pa + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}

	// Call the entry point
	entry = (void(*)(void))(elf->e_entry);
	entry();

fail:
	while (1);
}


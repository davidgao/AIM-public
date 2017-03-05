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

#ifndef _ASM_GENERIC_VMAIM_LDS_H
#define _ASM_GENERIC_VMAIM_LDS_H

#define LOAD_OFFSET	(KERN_BASE - KERN_PHYSBASE)

#define AT_OFFSET(sec)	AT(ADDR(sec) - LOAD_OFFSET)

/* 8 byte is the maximum alignment a function may require. */
#define ALIGN_FUNCTION() \
	. = ALIGN(8)

/* And a structure alignment as well */
#define STRUCT_ALIGNMENT 32
#define STRUCT_ALIGN() . = ALIGN(STRUCT_ALIGNMENT)

#define HIGH_SECTION(sec) \
	sec : AT_OFFSET(sec)

#define TEXT								\
	ALIGN_FUNCTION();						\
	*(.text)							\

#define TEXT_LOW							\
	ALIGN_FUNCTION();						\
	*(.text.low)

#define DATA(align)							\
	. = ALIGN((align));						\
	*(.data)

#define __INIT_SECTIONS(sec, align)					\
	. = ALIGN((align));						\
	sec##_init_start = .;						\
	*(.init.##sec##0)						\
	*(.init.##sec##1)						\
	*(.init.##sec##2)						\
	*(.init.##sec##3)						\
	*(.init.##sec##4)						\
	*(.init.##sec##5)						\
	*(.init.##sec##6)						\
	*(.init.##sec##7)						\
	sec##_init_end = .;

#define __INIT_SECTION(sec, align)					\
	. = ALIGN((align));						\
	sec##_init_start = .;						\
	*(.init.##sec)							\
	sec##_init_end = .;

#define EARLY_INIT(align)	__INIT_SECTION(early, align)
/* NORM is added as indication */
#define NORM_INIT(align)	__INIT_SECTIONS(norm, align)
#define LATE_INIT(align)	__INIT_SECTION(late, align)

#define RODATA(align)							\
	. = ALIGN((align));						\
	*(.rodata)

#define BSS(align)							\
	. = ALIGN((align));						\
	*(.bss)								\
	*(COMMON)

#endif /* _ASM_GENERIC_VMAIM_LDS_H */


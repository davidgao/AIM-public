/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 * Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef __ASSEMBLER__
/*
 * This macro is a wrapper for casting integer constants to unsigned longs,
 * freeing the code from compiler warnings and assembler errors (GCC
 * warns about shift count if a "UL" suffix is not appended while GAS
 * refuses to recognize the "UL" suffix).
 */
#define __ULCAST(i)	(i##UL)
#else	/* __ASSEMBLER__ */
#define __ULCAST(i)	i
#endif	/* !__ASSEMBLER__ */

#ifndef __ASSEMBLER__

#include <libc/stddef.h>

/* 
 * Type usage should be unified across AIM.
 * When a foreign module is imported, it MUST either apply AIM type naming, 
 * or provide typedefs by itself. Normally typedefs SHOULD NOT be added to
 * this header.
 */

typedef unsigned char		uint8_t;
typedef signed char		    int8_t;
typedef unsigned char       uchar;
typedef unsigned short		uint16_t;
typedef signed short		int16_t;
typedef unsigned short      ushort;
typedef unsigned int		uint32_t;
typedef unsigned int        uint;
typedef signed int		int32_t;
typedef unsigned long long	uint64_t;
typedef signed long long	int64_t;

typedef unsigned long ulong;

typedef unsigned int bool;
#define false	0
#define true	1

typedef void *uintptr_t;

typedef uint32_t atomic_t;
typedef int32_t satomic_t;

/* For in-memory objects, same as POSIX */
typedef unsigned long size_t;
typedef signed long ssize_t;

typedef unsigned long off_t;
typedef signed long soff_t;

/*
 * For use with other address space, like physical address space,
 * IO address space, etc. Note that 64-bit address spaces may present even on
 * 32-bit systems.
 * Use the unsigned version to indicate address and size,
 * the signed version to indicate offset.
 */
typedef unsigned long long addr_t;
typedef signed long long saddr_t;
typedef unsigned long long lsize_t, loff_t;
typedef signed long long slsize_t, sloff_t;

typedef int pid_t;

typedef int uid_t, gid_t, mode_t;

/*
 * Define types used within the system.
 * This is only a design pattern.
 */
typedef ulong	dev_t;
/*
 * dev_t, major, minor conversions
 *
 * dev_t used to be 16-bit, and major number takes the higher 8 bits, while
 * minor takes the lower ones.
 *
 * On Linux and BSD's dev_t was extended to 32-bit, and to comply with old
 * Unix behavior, Linux and BSD's used higher 16 bits in a very twisted
 * manner (for Linux, see /usr/include/sys/sysmacros.h).  The difference
 * of such usage between Linux and BSD's further made things worse.
 *
 * In AIM, we do not consider major and minor numbers exceeding 255, to
 * comply with both Linux and BSD while keeping logics cleaner.  Note that
 * we do NOT enforce range checks over the numbers (yet).
 */
static inline unsigned int major(dev_t dev)
{
	return ((dev >> 8) & 0xff);
}
static inline unsigned int minor(dev_t dev)
{
	return (dev & 0xff);
}
static inline dev_t makedev(unsigned int major, unsigned int minor)
{
	return ((major << 8) | (minor & 0xff));
}

typedef uint64_t ino_t;

/* A generic void function pointer type, allow any number of arguments */
typedef void (*generic_fp)();
typedef void (*callback_fp)();

#endif

#endif


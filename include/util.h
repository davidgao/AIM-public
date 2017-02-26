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

/* Kernel utility functions
 * Put miscellaneous functions and macros here */

#ifndef _UTIL_H
#define _UTIL_H

#ifdef __LP64__
#define WORD_SHIFT	3
#define WORD_SIZE	8
#define BITS_PER_LONG	64
#define BITS_PER_LONG_LOG	6
#define BITS_PER_LONG_MASK	63
#else
#define WORD_SHIFT	2
#define WORD_SIZE	4
#define BITS_PER_LONG	32
#define BITS_PER_LONG_LOG	5
#define BITS_PER_LONG_MASK	31
#endif

#ifndef __ASSEMBLER__

#include <sys/types.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define widthof(t)	(sizeof(t) * 8)

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))

#define min2(a, b)	(((a) < (b)) ? (a) : (b))
#define max2(a, b)	(((a) > (b)) ? (a) : (b))
#define min3(a, b, c)	min2((a), min2((b), (c)))
#define max3(a, b, c)	max2((a), max2((b), (c)))

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))

#define BITS_TO_LONGS(n)	DIV_ROUND_UP(n, sizeof(unsigned long))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)

#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)


/* Note: ALIGNxxx are suited for powers of 2 only, while
 * ROUNDxxx can deal with any integer value. */
#define __ALIGN_MASK(x, a)	((typeof(x))((a) - 1))
#define IS_ALIGNED(x, a)	(((x) & __ALIGN_MASK(x, a)) == 0)
#define ALIGN_ABOVE(x, a)	((((x) - 1) | __ALIGN_MASK(x, a)) + 1)
#define ALIGN_BELOW(x, a)	((x) & ~__ALIGN_MASK(x, a))
#define ALIGN_PREV(x, a)	ALIGN_BELOW((x) - 1, a)
#define ALIGN_NEXT(x, a)	ALIGN_ABOVE((x) + 1, a)
#define PTR_ALIGN_ABOVE(p, a)	\
	((typeof(p))ALIGN_ABOVE((unsigned long)(p), (a)))
#define PTR_ALIGN_BELOW(p, a)	\
	((typeof(p))ALIGN_BELOW((unsigned long)(p), (a)))
#define PTR_ALIGN_PREV(p, a)	\
	((typeof(p))ALIGN_PREV((unsigned long)(p), (a)))
#define PTR_ALIGN_NEXT(p, a)	\
	((typeof(p))ALIGN_NEXT((unsigned long)(p), (a)))
#define PTR_IS_ALIGNED(p, a)	\
	IS_ALIGNED((unsigned long)(p), (a))

#define ROUNDUP(x, d)		(DIV_ROUND_UP(x, d) * (d))
#define ROUNDDOWN(x, d)		((x) - ((x) % (d)))
#define ROUND_CLOSEST(x, d)	(((x) + ((d) / 2)) / (d))

/* binary operations on unsigned integral types */
#define get_lowest_0(x) ({\
	typeof(x) _x = x; \
	int i = 0; \
	if (_x + 1 == 0) i = -1; \
	else while ((_x & 1) == 1) { \
		i += 1; \
		_x >>= 1; \
	} \
	i; \
})

#define OVERLAP(st1, len1, st2, len2) \
	(((st1) > (st2) && (st1) < (st2) + (len2)) || \
	((st2) > (st1) && (st2) < (st1) + (len1)))

#define ADDR_CAST(x)		((size_t)(x))
#define ULCAST(x)		((size_t)(x))
#define PTRCAST(x)		((void *)ULCAST(x))

#else	/* __ASSEMBLER__ */

#define ULCAST(x)		(x)
#define PTRCAST(x)		(x)

#endif	/* !__ASSEMBLER__ */

#endif

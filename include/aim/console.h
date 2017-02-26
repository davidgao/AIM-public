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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#ifndef __ASSEMBLER__

typedef int (*putchar_fp)(int c);
typedef int (*puts_fp)(const char *s);

struct bus_device;

int __early_console_init(struct bus_device *bus, addr_t base, addr_t mapped_base);
int early_console_init(struct bus_device *bus, addr_t base, int type);
#define MAP_NONE	0
#define MAP_LINEAR	1
#define MAP_DYNAMIC	2

/* set_console(putchar, puts)
 * Register two routines to use as kernel console
 * These function pointers need to be in current address space.
 * Registered routines should be compiled as PIE and will work in both
 * address spaces.
 */
void set_console(putchar_fp putchar, puts_fp puts);


/* kernel console output routines
 * these will work in both address spaces.
 */
int kprintf(const char *fmt, ...);
int kputchar(int c);
int kputs(const char *s);	/* Atomic */
puts_fp get_default_kputs(void);
#define DEFAULT_KPUTS (get_default_kputs())

#ifdef DEBUG
#define kpdebug(...) kprintf("DEBUG: " __VA_ARGS__)
#else
#define kpdebug(...)
#endif

#endif /* !__ASSEMBLER__ */

#endif /* _CONSOLE_H */


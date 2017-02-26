/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#ifndef _PANIC_H
#define _PANIC_H

#ifndef __ASSEMBLER__

/*
 * Internal arch-independent code for panicking current processor.
 * May be called from arch code.
 */
__noreturn
void __local_panic(void);

__noreturn
void panic(const char *fmt, ...);

/* Arch/mach-dependent code */
void panic_other_cpus(void);

#endif /* !__ASSEMBLER__ */

#endif


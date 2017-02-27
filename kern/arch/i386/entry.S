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

.text

.globl master_early_entry

.globl kern_entry
kern_entry = master_early_entry - KERN_BASE

master_early_entry:
    cli     // no interrupt
    cld

    mov	$__kstack_top - KERN_BASE, %esp
    xor	%eax, %eax
    push	%eax		// eip=0
    push	%eax		// ebp=0
    mov %esp, %ebp
    call clear_bss_kern

    call master_early_init

    pop %ebp
    ret
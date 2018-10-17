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

.section .entry

.globl _start
_start:
	/* Clear Interupt */
	cli
	/* Clear Directory */
	cld
	/* Clear Segment Registers */
	xor	%ds, %ds
	xor	%es, %es
	xor	%ss, %ss
	/* Disable A20 Compatability */
seta20.1:
	in	$0x64, %al
	test	$0x2, %al
	jnz	seta20.1
	mov	$0xd1, %al
	out	%al, $0x64
seta20.2:
	in	$0x64, %al
	test	$0x2, %al
	jnz	seta20.2
	mov	$0xdf, %al
	out	%al, $0x60
	/* Load GDT and Start Protected Mode */
	lgdt	gdtdesc
	mov	%cr0, %eax
	or	$CR0_PE, %eax
	mov	%eax, %cr0
	ljmp	$(SEG_KCODE << 3), $bootasm

.text

bootasm:
	/* Set Up Segments */
	mov	$(SEG_KDATA<<3), %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %ss
	xor	%ax, %ax
	mov	%ax, %fs
	mov	%ax, %gs
	/* Set Up Stack */
	mov	$stacktop, %esp
	call	bootmain
	/* Spin in Case of Return */
spin:
	jmp	spin
	
.data
.p2align 2
gdt:
	SEG_NULL				// null seg
	SEG(STA_X|STA_R, 0x0, 0xffffffff)	// code seg
	SEG(STA_W, 0x0, 0xffffffff)		// data seg

gdtdesc:
	.word	(gdtdesc - gdt - 1)	// sizeof(gdt) - 1
	.long	gdt			// address of gdt

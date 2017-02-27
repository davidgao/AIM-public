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

#define __LDSCRIPT__

/*
 * Using the C preprocessor, we allow includes and macro expansion in this
 * linker script.
 */

ENTRY(kern_entry)
SECTIONS
{
    // Note: config define KERN_START as 0x01000000
    // or KERN_BASE 0x80000000

	. = KERN_BASE + 0x100000;
    __text_start_kern = .;
	.text   : AT(0x100000) {*(.text);}
	__text_end_kern = .;
	
	// . = ALIGN(1024 * 1024 * 4); // ensure seperate pages even in 4M paging
	.data   : 
	    // AT(LOADADDR(.text) + SIZEOF(.text)) 
	    {*(.data);}

    .rodata :
        // AT(LOADADDR(.bss) + SIZEOF(.bss)) 
        {*(.rodata);}

	.bss    : 
	    // AT(LOADADDR(.data) + SIZEOF(.data))
	{
        __bss_start_kern = .; 
        *(.bss);
        . = ALIGN(4096);
        PROVIDE(__kstack_base = .); 
        // using PROVIDE to get glb var
        . += KSTACKSIZE;
        // Note: config defines KSTACKSIZE as 32768(0x8000)
        PROVIDE(__kstack_top = .);
        __bss_end_kern = .;
    }

	.end = .;
	PROVIDE(__end = .);
	
}
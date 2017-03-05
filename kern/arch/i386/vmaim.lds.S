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

#include <asm-generic/vmaim.lds.h>

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
	.text   : AT(0x100000) {
		*(.text);
	}
	__text_end_kern = .;
	.rodata :
        {*(.rodata);}
	
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}
	
	.data   : 
	    {*(.data);}

	.bss    : 
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

    .init : {
    	EARLY_INIT(8);
    	NORM_INIT(8);
    	LATE_INIT(8);
    }

    . = ALIGN(4096);
	.end = .;
	PROVIDE(__end = .);

	. += (4096 * 16);
	PROVIDE(__early_buf_end = .);
	
}
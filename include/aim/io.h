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

#ifndef _AIM_IO_H
#define _AIM_IO_H

#ifndef __ASSEMBLER__

/*
 * These routines access direct-attached memory. All access are routed THROUGH
 * MMU if it is turned on. 
 */

static inline uint8_t read8(uint32_t addr)
{ return *(volatile uint8_t *)ULCAST(addr); }

static inline uint16_t read16(uint32_t addr)
{ return *(volatile uint16_t *)ULCAST(addr); }

static inline uint32_t read32(uint32_t addr)
{ return *(volatile uint32_t *)ULCAST(addr); }


static inline void write8(uint32_t addr, uint8_t data)
{ *(volatile uint8_t *)ULCAST(addr) = data; }

static inline void write16(uint32_t addr, uint16_t data)
{ *(volatile uint16_t *)ULCAST(addr) = data; }

static inline void write32(uint32_t addr, uint32_t data)
{ *(volatile uint32_t *)ULCAST(addr) = data; }

#endif /* !__ASSEMBLER__ */

/* in case the architecture has more to say, like IO ports */
#include <arch-io.h>

#endif /* _AIM_IO_H */


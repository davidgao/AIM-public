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

#ifndef _AIM_BOOT_H
#define _AIM_BOOT_H

#ifndef __ASSEMBLER__

extern uint8_t mbr[];

void readsect(void *dst, off_t offset);

#endif /* !__ASSEMBLER__ */

#include <arch-boot.h>

#ifndef SECT_SIZE
#define SECT_SIZE	512
#endif /* !SECT_SIZE */

#endif /* !_AIM_BOOT_H */


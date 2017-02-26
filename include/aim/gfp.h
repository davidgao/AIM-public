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

#ifndef _AIM_GFP_H
#define _AIM_GFP_H

/*
 * gfp.h provides various flags controlling behaviors or providing constraints
 * for allocating/freeing.
 */
#ifndef __ASSEMBLER__
typedef uint32_t gfp_t;
#endif /* !__ASSEMBLER__ */

#define GFP_UNSAFE	0x1	/* Do not fill in junk before freeing */
#define GFP_ZERO	0x2	/* Zero out the buffer after allocation */

#endif /* !_AIM_GFP_H */


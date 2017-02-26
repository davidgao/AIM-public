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

#ifndef _AIM_DEBUG_H
#define _AIM_DEBUG_H

#ifndef __ASSEMBLER__

/* 
 * We just want several function here, avoid include hell please.
 */
__noreturn
void panic(const char *fmt, ...);

#define assert(condition) \
	do { \
		if (!(condition)) \
			panic("Assertion failed in %s (%s:%d): %s\n", \
			    __func__, __FILE__, __LINE__, #condition); \
	} while (0)

#endif /* !__ASSEMBLER__ */

#endif /* !_AIM_DEBUG_H */


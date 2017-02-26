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

#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#include <sys/types.h>

/*
 * Kernel hyper-parameters which probably should not reside in configure script
 */

#define NODEV	((dev_t)(-1))
#define NOMAJOR	((unsigned int)(-1))
#define MAJOR_MAX	16
#define DEVICE_MAX	32
#define SECTOR_SIZE	512
#define JUNKBYTE	0x20

#endif

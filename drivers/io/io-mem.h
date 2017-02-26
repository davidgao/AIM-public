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

#ifndef _DRIVERS_IO_MEM_H
#define _DRIVERS_IO_MEM_H

#ifdef IO_MEM_ROOT
__attribute__ ((visibility ("hidden")))
extern struct bus_device early_memory_bus;
#endif /* IO_MEM_ROOT */

int io_mem_init(struct bus_device *memory_bus);

#endif /* _DRIVERS_IO_MEM_H */


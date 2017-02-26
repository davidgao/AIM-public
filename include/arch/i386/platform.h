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

#ifndef _PLATFORM_H
#define _PLATFORM_H

/* port base for x86 UART */
#define COM1		0x3f8

/* for UART */
#define UART_BASE	COM1
#define UART_FREQ	1843200

#define EARLY_CONSOLE_BUS	(&early_port_bus)
#define EARLY_CONSOLE_BASE	UART_BASE
#define EARLY_CONSOLE_MAPPING	MAP_NONE

#endif


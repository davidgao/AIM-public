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

#include <sys/types.h>
#include <aim/console.h>
#include <aim/device.h>
#include <aim/early_kmmap.h>
#include <aim/init.h>
#include <aim/mmu.h>
#include <aim/panic.h>
#include <drivers/io/io-mem.h>
#include <drivers/io/io-port.h>
#include <platform.h>

static inline
int early_devices_init(void)
{
#ifdef IO_MEM_ROOT
	if (io_mem_init(&early_memory_bus) < 0)
		return EOF;
#endif /* IO_MEM_ROOT */

#ifdef IO_PORT_ROOT
	if (io_port_init(&early_port_bus) < 0)
		return EOF;
#endif /* IO_PORT_ROOT */
	return 0;
}

__noreturn
void master_early_init(void)
{
	/* clear address-space-related callback handlers */
	early_mapping_clear();
	mmu_handlers_clear();
	/* prepare early devices like memory bus and port bus */
	if (early_devices_init() < 0)
		goto panic;
	/* other preperations, including early secondary buses */
	arch_early_init();
	if (early_console_init(
		EARLY_CONSOLE_BUS,
		EARLY_CONSOLE_BASE,
		EARLY_CONSOLE_MAPPING
	) < 0)
		panic("Early console init failed.\n");
	kputs("Hello, world!\n");

	goto panic;

panic:
	while (1);
}


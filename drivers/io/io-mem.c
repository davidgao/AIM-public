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

/*
 * This driver is a wrapper to access memory as a bus.
 * It may sound silly, but is actually needed if there's memory-attached
 * devices, especially when there's memory not connected to the processor
 * cores directly.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/param.h>
#include <aim/device.h>
#include <aim/gfp.h>
#include <aim/io.h>
#include <aim/mmu.h>
#include <aim/panic.h>
#include <errno.h>

#include <io-mem.h>

static int __read8(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	*ptr = (uint64_t)read8(base + offset);
	return 0;
}

static int __read16(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	*ptr = (uint64_t)read16(base + offset);
	return 0;
}

static int __read32(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	*ptr = (uint64_t)read32(base + offset);
	return 0;
}

static int __write8(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	write8(base + offset, (uint8_t)val);
	return 0;
}

static int __write16(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	write16(base + offset, (uint16_t)val);
	return 0;
}

static int __write32(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	write32(base + offset, (uint32_t)val);
	return 0;
}

static bus_read_fp __get_read_fp(struct bus_device *inst, int data_width)
{
	switch (data_width) {
		case 8: return __read8;
		case 16: return __read16;
		case 32: return __read32;
	}
	return NULL;
}

static bus_write_fp __get_write_fp(struct bus_device *inst, int data_width)
{
	switch (data_width) {
		case 8: return __write8;
		case 16: return __write16;
		case 32: return __write32;
	}
	return NULL;
}

#ifndef RAW
static void __jump_handler(void)
{
	early_memory_bus.bus_driver.get_read_fp = __get_read_fp;
	early_memory_bus.bus_driver.get_write_fp = __get_write_fp;
}
#endif

/* Should only be used before memory management is initialized */
struct bus_device early_memory_bus = {
	.addr_width = 32,
	.class = DEVCLASS_BUS,
	.name = "memory",
};

int io_mem_init(struct bus_device *memory_bus)
{
	memory_bus->bus_driver.get_read_fp = __get_read_fp;
	memory_bus->bus_driver.get_write_fp = __get_write_fp;
#ifndef RAW
	if (jump_handlers_add((generic_fp)__jump_handler) != 0)
		return EOF;
#endif /* RAW */
	return 0;
}

#ifndef RAW

#define DEVICE_MODEL	"io-mem"
#if 0
static struct bus_driver drv;

static int __new(struct devtree_entry *entry)
{
	/* default implementation... */
	return -EEXIST;
}

static struct bus_driver drv = {
	.class = DEVCLASS_BUS,
	.get_read_fp = __get_read_fp,
	.get_write_fp = __get_write_fp,
	.new = __new,
};

static int __driver_init(void)
{
	struct bus_device *memory_bus;
	register_driver(NOMAJOR, &drv);
#ifdef IO_MEM_ROOT
	memory_bus = kmalloc(sizeof(*memory_bus), GFP_ZERO);
	initdev(memory_bus, DEVCLASS_BUS, "memory", NODEV, &drv);
	dev_add(memory_bus);
#endif
	return 0;
}
INITCALL_DRIVER(__driver_init);
#endif /* 0 */
#endif /* !RAW */


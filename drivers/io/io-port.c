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

#include <io-port.h>

#ifdef IO_PORT_ROOT
#include <asm.h>
#endif /* IO_PORT_ROOT */
/*#include <sys/param.h>
#include <aim/initcalls.h>
#include <util.h>
#include <asm-generic/funcs.h>*/

//#include <asm.h>	/* inb() and outb() should be declared there */
/*#include <io-port.h>
#include <errno.h>*/

/*
 * To determine whether we should interact with port I/O bus directly or
 * via memory bus, we check if the bus instance is connected to another
 * bus.
 *
 * For architectures without ways to directly interact with port I/O bus,
 * or more precisely, architectures without IN/OUT instructions,
 * functions inb(), outb(), etc. are **REQUIRED** to cause a panic.
 *
 * If the driver framework works correctly, port-mapped I/O through
 * address space should be redirected to memory bus, so the 'else'
 * branches in read/write functions below are never reached.
 */
static int __read8(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read8;

	if (bus) {
		bus_read8 = bus->bus_driver.get_read_fp(bus, 8);
		bus_read8(bus, inst->base, base + offset, ptr);
	} else {
#ifdef IO_PORT_ROOT
		*ptr = in8((uint16_t)(base + offset));
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
	}

	return 0;
}

static int __write8(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write8;

	if (bus) {
		bus_write8 = bus->bus_driver.get_write_fp(bus, 8);
		return bus_write8(bus, inst->base, base + offset, val);
	} else {
#ifdef IO_PORT_ROOT
		out8((uint16_t)(base + offset), val);
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
		return 0;
	}
}

static int __read16(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read16;

	if (bus) {
		bus_read16 = bus->bus_driver.get_read_fp(bus, 16);
		bus_read16(bus, inst->base, base + offset, ptr);
	} else {
#ifdef IO_PORT_ROOT
		*ptr = in16((uint16_t)(base + offset));
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
	}

	return 0;
}

static int __write16(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write16;

	if (bus) {
		bus_write16 = bus->bus_driver.get_write_fp(bus, 16);
		return bus_write16(bus, inst->base, base + offset, val);
	} else {
#ifdef IO_PORT_ROOT
		out16((uint16_t)(base + offset), val);
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
		return 0;
	}
}

static int __read32(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read32;

	if (bus) {
		bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
		bus_read32(bus, inst->base, base + offset, ptr);
	} else {
#ifdef IO_PORT_ROOT
		*ptr = in32((uint16_t)(base + offset));
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
	}

	return 0;
}

static int __write32(struct bus_device *inst, addr_t base, addr_t offset,
    uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32;

	if (bus) {
		bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
		return bus_write32(bus, inst->base, base + offset, val);
	} else {
#ifdef IO_PORT_ROOT
		out32((uint16_t)(base + offset), val);
#else
		panic("Invalid IO port bus routine invoked.\n");
#endif /* IO_PORT_ROOT */
		return 0;
	}
}

static bus_read_fp __get_read_fp(struct bus_device *inst, int data_width)
{
	switch (data_width) {
	case 8:
		return __read8;
	case 16:
		return __read16;
	case 32:
		return __read32;
	}
	return NULL;
}

static bus_write_fp __get_write_fp(struct bus_device *inst, int data_width)
{
	switch (data_width) {
	case 8:
		return __write8;
	case 16:
		return __write16;
	case 32:
		return __write32;
	}
	return NULL;
}

void portio_bus_connect(struct bus_device *portio,
			struct bus_device *bus,
			addr_t base)
{
	portio->base = base;
	portio->bus = bus;
}

static void __portio_bus_init(struct bus_device *portio)
{
	portio->bus_driver.get_read_fp = __get_read_fp;
	portio->bus_driver.get_write_fp = __get_write_fp;
}

/*
 * IMPORTANT NOTE:
 * Port I/O bus structure should be further initialized in
 * machine-dependent code in case of accessing ports via
 * address spaces.
 */
struct bus_device early_port_bus = {
	/* FIXME ? */
	.addr_width = 32,
	.class = DEVCLASS_BUS,
};

#ifndef RAW
static void __jump_handler(void)
{
	__portio_bus_init(&early_port_bus);
}
#endif /* RAW */

int io_port_init(struct bus_device *port_bus)
{
	__portio_bus_init(port_bus);
#ifndef RAW
	if (jump_handlers_add((generic_fp)__jump_handler) != 0)
		return EOF;
#endif /* RAW */
	return 0;
}

#ifndef RAW

#define DEVICE_MODEL	"io-port"
#if 0
static struct bus_driver drv;

static int __new(struct devtree_entry *entry)
{
	struct bus_device *dev;
	if (strcmp(entry->model, DEVICE_MODEL) != 0)
		return -ENOTSUP;
	kpdebug("initializing I/O bus\n");
	dev = kmalloc(sizeof(*dev), GFP_ZERO);
	if (dev == NULL)
		return -ENOMEM;
	initdev(dev, DEVCLASS_BUS, entry->name, NODEV, &drv);
	dev->bus = (struct bus_device *)dev_from_name(entry->parent);
	dev->base = entry->regs[0];
	dev->nregs = entry->nregs;
	dev_add(dev);
	return 0;
}

static struct bus_driver drv = {
	.class = DEVCLASS_BUS,
	.get_read_fp = __get_read_fp,
	.get_write_fp = __get_write_fp,
	.new = __new,
};

static int __driver_init(void)
{
	struct bus_device *portio_bus;
	register_driver(NOMAJOR, &drv);
#ifdef IO_PORT_ROOT
	portio_bus = kmalloc(sizeof(*portio_bus), GFP_ZERO);
	initdev(portio_bus, DEVCLASS_BUS, "portio", NODEV, &drv);
	dev_add(portio_bus);
#endif
	return 0;
}
INITCALL_DRIVER(__driver_init);
#endif /* 0 */
#endif /* !RAW */


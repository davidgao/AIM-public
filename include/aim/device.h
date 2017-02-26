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

#ifndef _DEVICE_H
#define _DEVICE_H

//#include <aim/sync.h>
#include <list.h>
#include <libc/stddef.h>

/* forward */
struct bus_device;
struct proc;	/* include/proc.h */
struct buf;	/* include/buf.h */
struct uio;	/* fs/uio.h */

struct devtree_entry;	/* see below */
struct device;		/* see below */

/* Drivers */
struct driver {
	int class;
#define DEVCLASS_NON	0
#define DEVCLASS_CHR	1
#define DEVCLASS_BLK	2
#define DEVCLASS_NET	3
#define DEVCLASS_BUS	4
	int type;
#define DRVTYPE_NORMAL	0
#define DRVTYPE_TTY	1
	int (*open)(dev_t dev, int oflags, struct proc *p);
	int (*close)(dev_t dev, int oflags, struct proc *p);
	/* the following is for internal use in driver framework only */
	/*
	 * Read a device tree entry structure and create a struct device
	 * accordingly.  Usually only used on concrete hardware rather
	 * than virtual devices (like "tty" or "zero").  Leave it NULL
	 * if not applicable.
	 */
	int (*new)(struct devtree_entry *);
};

struct chr_driver {
	struct driver;
	int (*read)(dev_t dev, struct uio *uio, int ioflags);
	int (*write)(dev_t dev, struct uio *uio, int ioflags);

	/*
	 * These methods are intended for internal use.  They should not
	 * be invoked outside the driver framework.
	 */
	int (*getc)(dev_t dev);
	int (*putc)(dev_t dev, int c);
};

struct blk_driver {
	struct driver;
	int (*strategy)(struct buf *bp);
};

struct net_driver {
	struct driver;
};

/* Bus subsystem details */

/*
 * Buses vary in address width, and may provide multiple data access width.
 * A single bus may have quite some read/write routines, and may even not come
 * in pairs.
 * Bus access may or may not encounter failures. In the latter case, access
 * routines simply return 0 to indicate a success.
 * TODO: explain why we fix buffer pointer as a uint64_t pointer.
 */
typedef int (*bus_read_fp)(struct bus_device *inst,
	addr_t base, addr_t offset, uint64_t *ptr);
typedef int (*bus_write_fp)(struct bus_device *inst,
	addr_t base, addr_t offset, uint64_t val);

/*
 * To get acess routines like listed above, a caller should ask for them.
 * Data width MUST be given. These routines may return now in and only in cases
 * that the underlying bus controller cannot handle the given data width.
 * A single bus cannot have multiple address widths, and the value is written
 * in struct bus_device.
 */

struct bus_driver {
	struct driver;
	bus_read_fp (*get_read_fp)(
		struct bus_device *inst, int data_width);
	bus_write_fp (*get_write_fp)(
		struct bus_device *inst, int data_width);
	/*
	 * probe:
	 * Scan the bus to find devices to be added.
	 * Does *NOT* create actual struct device's.  When a device is found,
	 * probe() calls discover_device() to add the device descriptor to
	 * the undriven device list.
	 */
	int (*probe)(struct bus_device *inst);
};

extern struct driver *devsw[];

void register_driver(unsigned int major, struct driver *drv);

/* Devices */
#define DEV_NAME_MAX		64
#define DEV_REG_MAX		10
#define DEV_INTRCELL_MAX	10
struct device {
	char name[DEV_NAME_MAX];

	int class;
	dev_t devno;

	struct bus_device *bus;
	union {
		/* Some devices (e.g. PC IDE hard disk) have multiple
		 * register spaces with different base addresses. */
		addr_t bases[DEV_REG_MAX];
		/* But most devices only have one.  In that case, we
		 * could as well use this member instead. */
		addr_t base;
	};
	int nregs;	/* number of register spaces */

	union {
		struct driver driver;
		struct chr_driver chr_driver;
		struct blk_driver blk_driver;
		struct net_driver net_driver;
		struct bus_driver bus_driver;
	};

	//lock_t lock;
};

struct chr_device {
	struct device;

	/* Character buffer for keyboard/serial console/etc. */
	struct cbuf {
		unsigned char buf[BUFSIZ];
		int head;
		int tail;
		//lock_t lock;
	} cbuf;
};

struct blk_device {
	struct device;

	struct list_head bufqueue;
	/* Reserved for later use */
};

struct net_device {
	struct device;

	/* Reserved for later use */
};

struct bus_device {
	struct device;
	int addr_width;
};

/* Managing devices */

struct device_index {
	int (*add)(struct device *dev);
	int (*remove)(struct device *dev);
	struct device *(*next)(struct device *dev, void **savep);
	struct device *(*from_id)(dev_t devno);
	struct device *(*from_name)(char *name);
};

void set_device_index(struct device_index *index);

int dev_add(struct device *dev);
int dev_remove(struct device *dev);
/* [TODO] should require the device list to be locked in order to avoid race
 * condition between dev_next and dev_add. */
struct device *dev_next(struct device *dev, void **savep);
struct device *dev_from_id(dev_t devno);
struct device *dev_from_name(char *name);
void initdev(struct device *dev, int class, const char *devname, dev_t devno,
    struct driver *drv);
void probe_devices(void);

/* iterate over all devices */
#define for_each_device(dev, savep) \
	for ((dev) = dev_next(NULL, &(savep)); \
	     (dev) != NULL; \
	     (dev) = dev_next((dev), &(savep)))

/*
 * Device tree structure
 *
 * This structure is a subset of Device Tree Specification.  Here I'm
 * assuming that:
 * 1. Only one integer is needed to describe an address (#address-cells == 1).
 * 2. Each device know its IRQ number before registering handler to kernel.
 *    (We do not implement IRQ detection here)
 *
 * The root device tree node could only be "memory" or "portio".
 */
struct devtree_entry {
	/* device name */
	char	name[DEV_NAME_MAX];
	/* device model, should be understood by one and only one driver */
	char	model[DEV_NAME_MAX];
	/* parent device (usually a bus) name */
	char	parent[DEV_NAME_MAX];
	/* number of register spaces on the parent bus */
	int	nregs;
	/* list of register space bases */
	addr_t	regs[DEV_REG_MAX];
	/* the IRQ number the device generates */
	int	irq;
};
extern struct devtree_entry devtree[];
extern int ndevtree_entries;	/* # of dev tree entries */
/* Bus probing method calls this function to add device to undriven device
 * entry list. */
void discover_device(struct devtree_entry *entry);

#endif /* _DEVICE_H */


/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#include <sys/param.h>
#include <sys/types.h>
#include <libc/string.h>

#include <aim/console.h>
#include <aim/debug.h>
#include <aim/device.h>
//#include <aim/sync.h>
#include <aim/vmm.h>
#include <errno.h>

/* drivers with associated major number */
struct driver *devsw[MAJOR_MAX];
#define DRIVERS_MAX	128
/* all drivers, including those without major numbers (e.g. bus drivers) */
static struct driver *drivers[DRIVERS_MAX];
static int ndrivers = 0;

struct devtree_entry_node {
	struct devtree_entry entry;
	struct list_head node;
};

static struct list_head __undriven_devs = EMPTY_LIST(__undriven_devs);
//static lock_t __undriven_devlock = EMPTY_LOCK(__undriven_devlock);

void register_driver(unsigned int major, struct driver *drv)
{
	if (major != NOMAJOR)
		devsw[major] = drv;
	drivers[ndrivers++] = drv;
}

void initdev(struct device *dev, int class, const char *name, dev_t devno,
    struct driver *drv)
{
	dev->devno = devno;
	dev->class = class;
	strlcpy(dev->name, name, DEV_NAME_MAX);
	//spinlock_init(&dev->lock);

	assert(dev->class == drv->class);

	switch (dev->class) {
	case DEVCLASS_CHR:
		memcpy(&dev->chr_driver, drv, sizeof(dev->chr_driver));
		break;
	case DEVCLASS_BLK:
		memcpy(&dev->blk_driver, drv, sizeof(dev->blk_driver));
		break;
	case DEVCLASS_NET:
		memcpy(&dev->net_driver, drv, sizeof(dev->net_driver));
		break;
	case DEVCLASS_BUS:
		memcpy(&dev->bus_driver, drv, sizeof(dev->bus_driver));
		break;
	case DEVCLASS_NON:
		memcpy(&dev->driver, drv, sizeof(dev->driver));
		break;
	default:
		panic("%s: unknown device class %d\n", __func__, dev->class);
	}
}

static bool __dev_match(struct device *dev, struct devtree_entry *entry)
{
	return ((dev->nregs == entry->nregs) &&
	    (dev->bus == (struct bus_device *)dev_from_name(entry->parent)));
}

static bool __entry_match(struct devtree_entry *a, struct devtree_entry *b)
{
	return ((a->nregs == b->nregs) &&
		(strcmp(a->parent, b->parent) == 0) &&
		(memcmp(a->regs, b->regs, sizeof(addr_t) * a->nregs) == 0));
}

static struct device *__check_dup_device(struct devtree_entry *entry)
{
	struct device *dev;
	void *savep;

	for_each_device (dev, savep) {
		if (__dev_match(dev, entry))
			return dev;
	}
	return NULL;
}

void discover_device(struct devtree_entry *entry)
{
	struct devtree_entry_node *entry_node;
	struct device *dev;
	//unsigned long flags;

	/*
	 * Skip cases where
	 * 1. The device for this entry is already created (e.g. bus probing
	 *    found the device *after* static creation from DTB).
	 * 2. The entry is already inside the undriven device entry list.
	 */
	if ((dev = __check_dup_device(entry)) != NULL)
		return;

	//spin_lock_irq_save(&__undriven_devlock, flags);

	for_each_entry (entry_node, &__undriven_devs, node) {
		if (__entry_match(&entry_node->entry, entry)) {
			//spin_unlock_irq_restore(&__undriven_devlock, flags);
			return;
		}
	}

	kpdebug("discovered device %s (model %s)\n", entry->name, entry->model);
	entry_node = kmalloc(sizeof(*entry_node), 0);
	entry_node->entry = *entry;
	list_add_tail(&entry_node->node, &__undriven_devs);

	//spin_unlock_irq_restore(&__undriven_devlock, flags);
}

static bool __probe_devices(void)
{
	struct devtree_entry *entry;
	struct devtree_entry_node *entry_node, *next;
	struct bus_device *bus;
	struct device *dev;
	void *savep;
	//unsigned long flags;
	bool found = false;
	int i;

	/* Scan the device tree to discover devices with known bus. */
	for (i = 0; i < ndevtree_entries; ++i) {
		entry = &devtree[i];
		if ((dev = dev_from_name(entry->name)) != NULL) {
			/* already initialized, skip */
			if (!__dev_match(dev, entry))
				panic("device mismatch DTB: %s\n", dev->name);
			continue;
		}

		bus = (struct bus_device *)dev_from_name(entry->parent);
		if (bus == NULL)
			continue;
		/*
		 * Check if there are any struct device with exactly the
		 * same bus and register spaces, only with a different name.
		 *
		 * This could happen if a device, having its own tree entry,
		 * is probed by a bus.  An example is that, a SATA controller
		 * is described in the device tree to be attached to a PCI
		 * bus, and the PCI bus driver already probed the same
		 * SATA controller (and already created a struct device).
		 */
		if ((dev = __check_dup_device(entry)) != NULL &&
				strcmp(entry->parent, "memory") != 0) {
			assert(strcmp(dev->name, entry->name) != 0);
			kprintf("KERN: overwriting device name %s with %s\n",
			    dev->name, entry->name);
			strlcpy(dev->name, entry->name, DEV_NAME_MAX);
			continue;
		}
		/* If the device tree tells us that a device should be
		 * at some place on some bus, we should add it to the
		 * undriven device entry list for pick up, since device
		 * tree never lie. */
		discover_device(&devtree[i]);
	}

	/* Scan all known buses which support active probing, and probe
	 * for new devices. */
	for_each_device (dev, savep) {
		if (dev->class == DEVCLASS_BUS &&
		    dev->bus_driver.probe != NULL)
			/* The probe() method will call discover_device()
			 * which adds the device tree entry into the
			 * undriven device entry list. */
			dev->bus_driver.probe((struct bus_device *)dev);
	}

	/* For each undriven device entry, ask all drivers to pick it up,
	 * create a struct device, and dev_add() it to the device list. */
	//spin_lock_irq_save(&__undriven_devlock, flags);
	for_each_entry_safe (entry_node, next, &__undriven_devs, node) {
		for (i = 0; i < ndrivers; ++i) {
			if (drivers[i]->new != NULL &&
			    drivers[i]->new(&entry_node->entry) == 0) {
				list_del(&entry_node->node);
				kfree(entry_node);
				found = true;
				break;
			}
		}
	}
	//spin_unlock_irq_restore(&__undriven_devlock, flags);

	/* If we created any device, we need to do the whole scanning again,
	 * since the newly created device could be a bus (and may hold
	 * more devices!) */
	return found;
}

void probe_devices(void)
{
	int i = 0;
	do {
		kpdebug("probing devices: pass %d\n", ++i);
	} while (__probe_devices());
}


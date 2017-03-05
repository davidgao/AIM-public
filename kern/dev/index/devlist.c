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
#include <libc/string.h>
#include <aim/console.h>
#include <aim/debug.h>
#include <aim/device.h>
#include <aim/initcalls.h>
//#include <aim/sync.h>
#include <aim/vmm.h>
#include <list.h>

struct device_entry {
	struct device *dev;
	struct list_head node;
};

static struct list_head __head = EMPTY_LIST(__head);
//static lock_t __lock;

/* comparition according to id */
static inline int __cmp(struct device_entry *a, struct device_entry *b)
{
	struct device *dev_a, *dev_b;
	dev_a = a->dev;
	dev_b = b->dev;
	if (dev_a->devno > dev_b->devno) return 1;
	if (dev_a->devno < dev_b->devno) return -1;
	return 0; /* unlikely */
}

static int __add(struct device *dev)
{
	struct device_entry *this, *tmp;

	/* allocate and fill in an entry */
	this = kmalloc(sizeof(struct device_entry), 0);
	if (this == NULL)
		return EOF;
	this->dev = dev;

	/* lock up the list */
//	spin_lock(&__lock);

	/* insert */
	for_each_entry(tmp, &__head, node) {
		if (__cmp(tmp, this) > 0) break;
	}
	list_add_before(&this->node, &tmp->node);

	/* unlock the list */
//	spin_unlock(&__lock);

	return 0;
}

static int __remove(struct device *dev)
{
	struct device_entry *tmp, *this = NULL;
	int retval = EOF;

	/* lock up the list */
//	spin_lock(&__lock);

	/* check for connected devices and the entry to remove */
	for_each_entry(tmp, &__head, node) {
		if ((struct device *)tmp->dev->bus == dev)
			goto ret; /* have connected device */
		else if (tmp->dev == dev)
			this = tmp;
	}

	/* remove */
	if (this != NULL) {
		list_del(&this->node);
		kfree(this);
		retval = 0; /* success */
	}

ret:
	/* unlock the list */
//	spin_unlock(&__lock);
	return retval;
}

static struct device *__next(struct device *dev, void **savep)
{
	if (dev == NULL) {
		*savep = list_first_entry(&__head, struct device_entry, node);
	} else {
		assert(((struct device_entry *)(*savep))->dev == dev);
		*savep = list_next_entry((struct device_entry *)(*savep),
		    struct device_entry, node);
	}
	if (*savep == &__head)
		return NULL;
	else
		return ((struct device_entry *)(*savep))->dev;
}

static struct device *__from_id(dev_t devno)
{
	struct device_entry *tmp;
	struct device *retval;

	/* lock up the list */
//	spin_lock(&__lock);

	/* loop and check */
	for_each_entry(tmp, &__head, node) {
		if (tmp->dev->devno == devno)
			break;
	}
	if (&tmp->node != &__head)
		retval = tmp->dev;
	else
		retval = NULL;

	/* unlock the list */
//	spin_unlock(&__lock);
	return retval;
}

static struct device *__from_name(char *name)
{
	struct device_entry *tmp;
	struct device *retval;

	/* lock up the list */
//	spin_lock(&__lock);

	/* loop and check */
	for_each_entry(tmp, &__head, node) {
		if (strcmp(tmp->dev->name, name) == 0)
			break;
	}
	if (&tmp->node != &__head)
		retval = tmp->dev;
	else
		retval = NULL;

	/* unlock the list */
//	spin_unlock(&__lock);
	return retval;
}

static int __init(void)
{
	kputs("KERN: <devlist> initializing.\n");

	struct device_index this = {
		.add		= __add,
		.remove		= __remove,
		.next		= __next,
		.from_id	= __from_id,
		.from_name	= __from_name
	};
	set_device_index(&this);

//	spinlock_init(&__lock);

	kputs("KERN: <devlist> Done.\n");
	return 0;
}


INITCALL_CORE(__init);


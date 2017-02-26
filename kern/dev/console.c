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

/* from kernel */
#include <sys/types.h>
#include <aim/console.h>
#include <aim/early_kmmap.h>
#include <aim/mmu.h>
//#include <aim/sync.h>
#include <libc/stdio.h>
#include <errno.h>

static putchar_fp __putchar = NULL;
static puts_fp __puts = NULL;
//static lock_t __lock;

/*
 * In most cases,
 * 1. Outputting a string to a console can be decomposed into
 *    outputting characters one-by-one.
 * 2. A new line on a console is a carriage return ('\r') followed
 *    by a line feed ('\n').
 * If your console device satisfies the two characteristics above,
 * you can use default kputs() implementation by
 *
 *     set_console(your_putchar, DEFAULT_KPUTS);
 *
 * Exceptions may exist, e.g. an efficient CGA console driver.  In
 * this case, set up console output functions by
 *
 *     set_console(your_putchar, your_puts);
 */
void set_console(putchar_fp putchar, puts_fp puts)
{
	__putchar = putchar;
	__puts = puts;
	//spinlock_init(&__lock);
}

int kprintf(const char *fmt, ...)
{
	int result;
	char printf_buf[BUFSIZ];
	va_list ap;
	va_start(ap, fmt);
	result = vsnprintf(printf_buf, BUFSIZ, fmt, ap);
	va_end(ap);
	kputs(printf_buf);
	return result;
}

int kputchar(int c)
{
	if (__putchar == NULL)
		return EOF;
	return __putchar(c);
}

static inline int __kputs(const char *s)
{
	if (__putchar == NULL)
		return EOF;

	for (; *s != '\0'; ++s) {
		if (*s == '\n')
			__putchar('\r');
		__putchar(*s);
	}

	return 0;
}

puts_fp get_default_kputs(void)
{
	return &__kputs;
}

int kputs(const char *s)
{
	int result;
	//unsigned long flags;

	if (__puts == NULL)
		return EOF;
	/* We probably don't want kputs() to be interrupted externally or by another
	 * core. */
	//spin_lock_irq_save(&__lock, flags);
	result = __puts(s);
	//spin_unlock_irq_restore(&__lock, flags);

	return result;
}

static inline int get_mapped_base(addr_t base, int type, addr_t *mapped_base)
{
	switch (type) {
		case MAP_NONE:
			*mapped_base = base;
			return 0;
		case MAP_LINEAR:
			*mapped_base = postmap_addr(base);
			return 0;
		case MAP_DYNAMIC:
			; // C grammar wants a statement here
			struct early_mapping entry = {
				.paddr = base,
				.size = EARLY_PAGE_SIZE,
				.type = EARLY_MAPPING_KMMAP,
			};
			int ret = early_mapping_add(&entry);
			if (ret < 0) return ret;
			*mapped_base = ULCAST(entry.vaddr);
			return 0;
		default:
			return -ENOTSUP;
	}
}

int early_console_init(struct bus_device *bus, addr_t base, int type)
{
	addr_t mapped_base;

	/* prepare mapped base address */
	int ret = get_mapped_base(base, type, &mapped_base);
	if (ret < 0) return ret; // caused by kmmap, may have various reasons

	/* really init console */
	return __early_console_init(bus, base, mapped_base);
}


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
#include <aim/panic.h>
#include <libc/stdarg.h>
#include <libc/stddef.h>
#include <libc/stdio.h>

/*
 * The rest place for every processor during a panic.
 *
 * __local_panic() is executed once per processor.
 */
__noreturn
void __local_panic(void)
{
	//local_irq_disable();
	/*
	 * We cannot simply do tight loops at panic. Modern kernels turn down
	 * processors and other devices to keep energy consumption and heat
	 * generation low.
	 * Later on there will be interfaces for the targets and drivers.
	 * We currently do a tight loop.
	 */

	for (;;)
		/* nothing */;
}

/*
 * The place where a panic starts.
 *
 * panic() is executed only on the processor throwing the panic.
 */
__noreturn
void panic(const char *fmt, ...)
{
	/*
	 * We might be in some strange state with limited stack, use a static
	 * buffer
	 */
	static char __buf[BUFSIZ];
	va_list args;
	int result;

	//local_irq_disable();

	//panic_other_cpus();

	va_start(args, fmt);
	result = vsnprintf(__buf, BUFSIZ, fmt, args);
	va_end(args);

	kputs("----- KERNEL PANIC -----\n");
	if (result >= BUFSIZ) {
		kputs("PANIC: message is truncated.");
	}
	kputs(__buf);
	
	__local_panic();
}


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

#ifndef _AIM_INITCALLS_H
#define _AIM_INITCALLS_H

typedef int (*initcall_t)(void);

#ifndef __ASSEMBLER__

#define __initcall(fn, sec) \
	static initcall_t __##sec##_initcall_##fn __used \
	__attribute__((__section__(".init." #sec))) = fn

/* In AIM, normal initcalls have stages. */
#define EARLY_INITCALL(fn) \
	__initcall(fn, early)

/* don't add NORM here as it should be the default pace to go */
#define INITCALL(fn, id) \
	__initcall(fn, norm##id)

#define LATE_INITCALL(fn) \
	__initcall(fn, late)

/* This is slightly different to linux. We assign a stage for rootfs. */
#define INITCALL_PURE(fn)	INITCALL(fn, 0)
#define INITCALL_CORE(fn)	INITCALL(fn, 1)
#define INITCALL_POSTCORE(fn)	INITCALL(fn, 2)
#define INITCALL_ARCH(fn)	INITCALL(fn, 3)
#define INITCALL_SUBSYS(fn)	INITCALL(fn, 4)
#define INITCALL_FS(fn)		INITCALL(fn, 5)
#define INITCALL_ROOTFS(fn)	INITCALL(fn, 6)
#define INITCALL_DEV(fn)	INITCALL(fn, 7)

#define INITCALL_SYSCALLS(fn)	INITCALL_SUBSYS(fn)
#define INITCALL_SCHED(fn)	INITCALL_SUBSYS(fn)
#define INITCALL_DRIVER(fn)	INITCALL_SUBSYS(fn)

int do_early_initcalls();
int do_initcalls();

#endif /* __ASSEMBLER__ */

#endif /* _AIM_INITCALLS_H */


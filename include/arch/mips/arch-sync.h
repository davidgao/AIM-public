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

#ifndef _ARCH_SYNC_H
#define _ARCH_SYNC_H

#ifndef __ASSEMBLER__

typedef unsigned int lock_t;
#define EMPTY_LOCK(lock)	(UNLOCKED)

static inline
void spinlock_init(lock_t *lock)
{
}

static inline
void spin_lock(lock_t *lock)
{
}

static inline
void spin_unlock(lock_t *lock)
{
}

static inline
bool spin_is_locked(lock_t *lock)
{
	return true;
}

/* Semaphore */
typedef struct {
	int val;
	int limit;
} semaphore_t;

static inline
void semaphore_init(semaphore_t *sem, int val)
{
}

static inline
void semaphore_dec(semaphore_t *sem)
{
}

static inline
void semaphore_inc(semaphore_t *sem)
{
}

#endif /* __ASSEMBLER__ */

#endif /* _ARCH_SYNC_H */


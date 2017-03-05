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

#ifndef _AIM_SYNC_H
#define _AIM_SYNC_H

#define UNLOCKED	0
#define LOCKED		1

#include <aim/irq.h>	/* local_irq_XXX */
#include <aim/panic.h>

#include <arch-sync.h>	/* lock_t */

#ifndef __ASSEMBLER__

/* Spinlocks. Implemented by architectures. */

/* By initializing a lock, caller assumes no code is holding it. */
static inline
void spinlock_init(lock_t *lock);
static inline
void spin_lock(lock_t *lock);
/* spin_unlock may contain instructions to send event */
static inline
void spin_unlock(lock_t *lock);
static inline
bool spin_is_locked(lock_t *lock);

#define spin_lock_irq_save(lock, flags) \
	do { \
		local_irq_save(flags); \
		spin_lock(lock); \
	} while (0)
#define spin_unlock_irq_restore(lock, flags) \
	do { \
		spin_unlock(lock); \
		local_irq_restore(flags); \
	} while (0)

/* Recursive spinlocks */

typedef struct {
	lock_t	lock;
	int	holder;	/* CPU # currently holding the lock */
	int	depth;	/* how many times lock()'d */
} rlock_t;

#define EMPTY_RLOCK(lk)	{ \
	.lock = EMPTY_LOCK((lk).lock), \
	.holder = -1, \
	.depth = 0 \
}

static inline void recursive_lock(rlock_t *lock)
{
	if (lock->holder != cpuid()) {
		spin_lock(&lock->lock);
		lock->holder = cpuid();
	}
	++lock->depth;
}

static inline void recursive_unlock(rlock_t *lock)
{
	assert(cpuid() == lock->holder);
	assert(lock->depth > 0);
	--lock->depth;
	if (lock->depth == 0) {
		lock->holder = -1;
		spin_unlock(&(lock->lock));
	}
}

#define recursive_lock_irq_save(lk, flags) \
	do { \
		local_irq_save(flags); \
		recursive_lock(lk); \
	} while (0)
#define recursive_unlock_irq_restore(lk, flags) \
	do { \
		recursive_unlock(lk); \
		local_irq_restore(flags); \
	} while (0)

/* Semaphore, implemented by architectures. */

static inline
void semaphore_init(semaphore_t *sem, int val);
static inline
void semaphore_dec(semaphore_t *sem);
/* semaphore_inc may contain instructions to send event */
static inline
void semaphore_inc(semaphore_t *sem);
#define semaphore_pass(sem) ({ \
	semaphore_t *_sem = sem; \
	semaphore_dec(_sem); \
	semaphore_inc(_sem); })

#endif /* !__ASSEMBLER__ */

#endif /* _AIM_SYNC_H */


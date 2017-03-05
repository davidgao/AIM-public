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

#include <aim/console.h>
#include <asm.h>
#include <proc.h>

typedef struct mutex {
	uint32_t locked;

	char *desc;
	struct cpu *cpu;
} lock_t;

#define LOCK_INITIALIZER 	{0, NULL, NULL}
// #define EMPTY_LOCK(lock)	(UNLOCKED)

static inline
void spinlock_init(lock_t *m)
{
	m->desc = NULL;
	m->locked = 0;
	m->cpu = NULL;
}

static inline
void spinlock_init2(lock_t *m, char *desc)
{
	m->desc = desc;
	m->locked = 0;
	m->cpu = NULL;
}

static bool holding(lock_t *m) {
	return m->locked && (m->cpu == get_gs_cpu());
}

static inline
void spin_lock(lock_t *m)
{
	// pushcli();
	if(holding(m))
		panic("acquire: trying to lock again");
	while(xchg(&m->locked, 1) != 0)
		;
	// __snyc_synchronize();
	m->cpu = get_gs_cpu();
}

static inline 
bool spin_lock_once(lock_t *m) {
	// pushcli();
	if(holding(m))
		panic("acquire: trying to lock again");
	while(xchg(&m->locked, 1) != 0)
		return 0;
	// __snyc_synchronize();
	m->cpu = get_gs_cpu();
	return 1;
}

static inline
void spin_unlock(lock_t *m)
{
	if(!holding(m))
		panic("release: Trying to release unacquired lock");
	m->cpu = NULL;
	// __snyc_synchronize();
	asm volatile("movl $0, %0" : "+m" (m->locked) : );
	// popcli();
}

static inline
bool spin_is_locked(lock_t *lock)
{
	return lock->locked;
}

/* Semaphore */
typedef struct {
	int val;
	int limit;
	lock_t mutex;

	char *desc;
} semaphore_t;

static inline
void semaphore_init(semaphore_t *sem, int val)
{
	if(val < 0) 
		val = 0;
	sem->val = sem->limit = val;
	spinlock_init(&sem->mutex);
	sem->desc = NULL;
}

static inline
void semaphore_init2(semaphore_t *sem, int val, char *desc)
{
	if(val < 0) 
		val = 0;
	sem->val = sem->limit = val;
	spinlock_init(&sem->mutex);
	sem->desc = desc;
}
#define MZYDEBUG
static inline
void semaphore_dec(semaphore_t *s)
{
	spin_lock(&s->mutex);
	if(s->val <= 0) {
re_ent:
		spin_unlock(&s->mutex);
		#ifdef MZYDEBUG
		int count = 0;
		bool count_enable = true;
		#endif 
		while(s->val <= 0){
			#ifdef MZYDEBUG
			count ++;
			if(count_enable && count > 0x1000) {
				kprintf("Warning | semaphore_dec: having waited 0x1000 loops for %s\n", s->desc);
				count_enable = false;
			}
			#endif
		}
		spin_lock(&s->mutex);
	}
	if(s->val <= 0)
		goto re_ent;
	s->val --;
	spin_unlock(&s->mutex);
}

static inline
void semaphore_inc(semaphore_t *s)
{
	spin_lock(&s->mutex);
	s->val ++;
	if(s->val > s->limit) {
		panic("semaphore_inc: inc overflow");
	}
	spin_unlock(&s->mutex);
}

#define SEM_INITIALIZER(x) {(x), (x), LOCK_INITIALIZER, NULL}

#endif /* __ASSEMBLER__ */

#endif /* _ARCH_SYNC_H */


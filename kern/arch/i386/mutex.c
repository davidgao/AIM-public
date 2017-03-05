#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <mutex.h>
#include <asm.h>
#include <proc.h>

void initlock(struct mutex *m, char *desc){
	m->desc = desc;
	m->locked = 0;
	m->cpu = NULL;
}

int single_acquire(struct mutex *m) {
	pushcli();
	if(holding(m))
		panic("single_acquire: trying to lock again");
	//while(xchg(&m->locked, 1) != 0)
	//	;
	if(xchg(&m->locked, 1) != 0) {
		popcli();
		return 0;
	}
	// __snyc_synchronize();
	m->cpu = get_gs_cpu();
	return 1;
}

int acquire(struct mutex *m) {
	pushcli();
	if(holding(m))
		panic("acquire: trying to lock again");
	//while(xchg(&m->locked, 1) != 0)
	//	;
	while(xchg(&m->locked, 1) != 0) {
		return 0;
	}
	// __snyc_synchronize();
	m->cpu = get_gs_cpu();
	return 1;
}

int release(struct mutex *m){
	if(!holding(m))
		panic("release: Trying to release unacquired lock");
	m->cpu = NULL;
	// __snyc_synchronize();
	asm volatile("movl $0, %0" : "+m" (m->locked) : );
	popcli();
	return 1;
}

bool holding(struct mutex *m) {
	return m->locked && m->cpu == get_gs_cpu();
}

void pushcli() {
	int eflags = readeflags();
	cli();

	struct cpu *cpu = get_gs_cpu();

	if(cpu->ncli == 0) {
		cpu->intena = eflags & FL_IF;
	}
	cpu->ncli ++;
}

void popcli() {
	if(readeflags() & FL_IF) 
		panic("popcli: Interruptable");

	struct cpu *cpu = get_gs_cpu();

	if(--cpu->ncli < 0)
		panic("popcli: Illegal behaviour");
	if(cpu->ncli == 0 && cpu->intena) {
		sti();
	}
}

// semaphore is here
char *SEM_LOCK_DESC = "semaphore lock";

void seminit(struct semaphore *s, int max, char *desc) {
	s->count = max;
	s->desc = desc;
	initlock(&s->lock, SEM_LOCK_DESC);
}

int semup(struct semaphore *s) {
	acquire(&s->lock);
	s->count ++;
	//TODO: wake up sth.
	release(&s->lock);
	return 1;
}

int single_semdown(struct semaphore *s) {
	acquire(&s->lock);
	if(s->count <= 0){
		//TODO: add wait queue
		return 0;
	}
	s->count --;
	release(&s->lock);
	return 1;
}

int semdown(struct semaphore *s) {
	// use waitlist in the future!
	return single_semdown(s);
}
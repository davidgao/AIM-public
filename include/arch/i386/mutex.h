#ifndef _MUTEX_H_
#define _MUTEX_H_ 

#ifndef __ASSEMBLER__
/*
#include <proc.h>
struct mutex {
	uint32_t locked;
	char *desc;
	struct cpu *cpu;
};
void initlock(struct mutex *m, char *desc);
int acquire(struct mutex *m);
int single_acquire(struct mutex *m);
int release(struct mutex *m);
bool holding(struct mutex *m);
void pushcli();
void popcli();
#define MUTEX_INITIALIZER {0, NULL, NULL}
struct semaphore {
	int count;
	struct mutex lock;
	char *desc;
	//TODO: waitlist
};
int semup(struct semaphore *s);
int semdown(struct semaphore *s);
int single_semdown(struct semaphore *s);
void seminit(struct semaphore *s, int max, char *desc);
#define SEM_INITIALIZER(x) {x, MUTEX_INITIALIZER, NULL}
*/

#endif	//__ASSEMBLER__

#endif
struct spinlock {
  uint32_t locked;       // Is the lock held?

  // For debugging:
  
  char *name;        // Name of lock
  
  /*    TODO: skip cpu for now
  struct cpu *cpu;   // The cpu holding the lock.
  uint32_t pcs[10];      // The call stack (an array of program counters)
  */
                     // that locked the lock.
};

void
initlock(struct spinlock *lk, char *name);

void
acquire(struct spinlock *lk);

void
release(struct spinlock *lk);

void
getcallerpcs(void *v, uint32_t pcs[]);

int
holding(struct spinlock *lock);

void
pushcli(void);

void
popcli(void);
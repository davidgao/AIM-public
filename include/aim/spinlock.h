struct spinlock {
  uint32_t locked;

  // For debugging:
  
  char *name;        // Name of lock
  
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
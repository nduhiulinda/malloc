#include <pthread.h>
typedef union lock{
  pthread_mutex_t pthread_lock;
  int riscv_lock;
} lock_t;

void mutex_lock(volatile lock_t* lock);
void mutex_unlock(volatile lock_t* lock);

volatile extern lock_t malloc_lock;

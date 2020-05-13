#include "spinlock.h"
#include "pthread.h"

#ifdef __riscv
void mutex_unlock_riscv(volatile int* lock){

  //TODO: your implementation here!
  return;
}

void mutex_lock_riscv(volatile int* lock){
  //TODO: your implementation here!
  return;
}
#endif

/*
 * mutex_lock(lock) will attempt to take the lock and will block until taken.
 * requires: calling thread does not already have the lock (otherwise, will deadlock)
 */
void mutex_lock(volatile lock_t* lock){
  #ifdef __riscv
  volatile int* lock2 = &(lock->riscv_lock);

  mutex_lock_riscv(lock2);
#else
  pthread_mutex_t* lock2 = (pthread_mutex_t*) &(lock->pthread_lock);
  pthread_mutex_lock(lock2);
  return;
  #endif
}

/*
  mutex_unlock(lock) releases a held lock.
  requires: calling thread already has the lock (otherwise, correctness is impaired)
 */

void mutex_unlock(volatile lock_t* lock){
  #ifdef __riscv
  volatile int* lock2 = &(lock->riscv_lock);
  mutex_unlock_riscv(lock2);
  #else
  pthread_mutex_t* lock2 = (pthread_mutex_t*) &(lock->pthread_lock);
  pthread_mutex_unlock(lock2);
  return;
  #endif
}

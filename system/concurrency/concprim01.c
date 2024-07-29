/*
* C library functions – <pthread.h>

int pthread_mutex_init (pthread_mutex_t *mutex, const pthread_mutexattr_t
*attr); Initialize a mutex based on initial attributes.

int pthread_mutex_destroy (pthread_mutex_t *mutex);
Destroy an allocated mutex.

int pthread_mutex_lock (pthread_mutex_t *mutex);
Acquire the specified mutex lock.

int pthread_mutex_trylock (pthread_mutex_t *mutex);
Try to acquire the mutex lock; return a non-zero value if unsuccessful.

int pthread_mutex_unlock (pthread_mutex_t *mutex);
Release the specified mutex lock.
*/
#include <assert.h>
#include <pthread.h>

int globalVar = 5;

void *increment(void *args) {
  pthread_mutex_t *mutex = (pthread_mutex_t *)args;

  pthread_mutex_lock(mutex);
  globalVar++;
  pthread_mutex_unlock(mutex);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t threads[2];
  pthread_mutex_t mutex;

  pthread_mutex_init(&mutex, NULL);

  assert(pthread_create(&threads[0], NULL, increment, &mutex) == 0);
  assert(pthread_create(&threads[1], NULL, increment, &mutex) == 0);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  assert(globalVar == 7);

  return 0;
}

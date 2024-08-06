#include <linux/futex.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define _GNU_SOURCE
#define NUM_THREADS 10

static inline int futex_wait(atomic_int* futex, int expected) {
  return syscall(SYS_futex, futex, FUTEX_WAIT, expected, NULL, NULL, 0);
}

static inline int futex_wake(atomic_int* futex, int count) {
  return syscall(SYS_futex, futex, FUTEX_WAKE, count, NULL, NULL, 0);
}

typedef struct {
  atomic_int value;
} futex_mutex_t;

futex_mutex_t mutex;

void futex_mutex_init(futex_mutex_t* mutex) { atomic_store(&mutex->value, 0); }

void futex_mutex_lock(futex_mutex_t* mutex) {
  int expected = 0;
  while (!atomic_compare_exchange_weak(&mutex->value, &expected, 1)) {
    while (atomic_load(&mutex->value) != 0) {
      futex_wait(&mutex->value, 1);
    }
    expected = 0;
  }
}

void futex_mutex_unlock(futex_mutex_t* mutex) {
  if (atomic_load(&mutex->value) != 1) {
    return;
  }
  atomic_store(&mutex->value, 0);
  futex_wake(&mutex->value, 1);
}

void* worker(void* arg) {
  int id = *(int*)arg;
  printf("Thread %d: Waiting to acquire lock...\n", id);

  futex_mutex_lock(&mutex);

  printf("Thread %d: Acquired lock!\n", id);
  sleep(1);

  printf("Thread %d: Releasing lock...\n", id);
  futex_mutex_unlock(&mutex);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];
  futex_mutex_init(&mutex);

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, worker, &ids[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  return 0;
}

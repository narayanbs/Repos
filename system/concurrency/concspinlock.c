#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  atomic_flag lock;
} spinlock_t;

void spinlock_init(spinlock_t *lock) { atomic_flag_clear(&lock->lock); }

void spinlock_lock(spinlock_t *lock) {
  while (atomic_flag_test_and_set(&lock->lock)) {
    usleep(10);
  }
}

void spinlock_unlock(spinlock_t *lock) { atomic_flag_clear(&lock->lock); }

int counter = 0;
spinlock_t lock;

void *increment(void *arg) {
  for (int i = 0; i < 1000000; ++i) {
    spinlock_lock(&lock);
    counter++;
    spinlock_unlock(&lock);
  }
  return NULL;
}

int main(void) {
  pthread_t threads[2];
  spinlock_init(&lock);

  for (int i = 0; i < 2; ++i) {
    if (pthread_create(&threads[i], NULL, increment, NULL)) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < 2; ++i) {
    pthread_join(threads[i], NULL);
  }
  printf("Final counter value: %d\n", counter);
  return 0;
}

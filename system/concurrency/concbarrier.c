/*
 * Reusable Barrier Implementation Using Semaphores
 *
 * This code implements a fully reusable barrier for N threads using
 * semaphores and a mutex. Each thread waits at the barrier until all
 * threads have reached it. After all threads have arrived, they proceed
 * together, and the barrier is automatically reset for reuse.
 *
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 3       // Number of threads
#define ROUNDS 2  // Number of times the barrier is reused

/* ----------------------- Barrier Structure ----------------------- */

typedef struct {
  int count;              // Number of threads currently at the barrier
  int n_threads;          // Total threads to wait for
  pthread_mutex_t mutex;  // Protects 'count'
  sem_t turnstile1;       // Blocks threads until all arrive
  sem_t turnstile2;       // Blocks threads until all leave (reset)
} barrier_t;

/* ----------------------- Barrier Functions ----------------------- */

/**
 * Initialize the reusable barrier
 *
 * @param b Pointer to the barrier
 * @param n Total number of threads to synchronize
 */
void barrier_init(barrier_t *b, int n) {
  b->count = 0;
  b->n_threads = n;
  pthread_mutex_init(&b->mutex, NULL);
  sem_init(&b->turnstile1, 0, 0);  // Initially closed
  sem_init(&b->turnstile2, 0, 1);  // Initially open for reset
}

/**
 * Wait at the barrier (two-phase reusable barrier)
 *
 * Threads block here until all N threads have reached the barrier.
 * After all threads arrive, they proceed together and the barrier
 * is reset automatically for reuse.
 *
 * @param b Pointer to the barrier
 */
void barrier_wait(barrier_t *b) {
  // -------- Phase 1: Arrival --------
  pthread_mutex_lock(&b->mutex);
  b->count++;
  if (b->count == b->n_threads) {
    // Last thread arrives → open turnstile1 for all threads
    for (int i = 0; i < b->n_threads; i++) sem_post(&b->turnstile1);
  }
  pthread_mutex_unlock(&b->mutex);

  sem_wait(&b->turnstile1);  // Wait for barrier to open

  // -------- Phase 2: Departure --------
  pthread_mutex_lock(&b->mutex);
  b->count--;
  if (b->count == 0) {
    // Last thread leaving → open turnstile2 for next use
    for (int i = 0; i < b->n_threads; i++) sem_post(&b->turnstile2);
  }
  pthread_mutex_unlock(&b->mutex);

  sem_wait(&b->turnstile2);  // Wait for barrier reset to complete
}

/* ----------------------- Example Usage ----------------------- */

barrier_t barrier;  // Global reusable barrier

void *thread_func(void *arg) {
  int id = *(int *)arg;

  for (int r = 0; r < ROUNDS; r++) {
    printf("Thread %d before barrier, round %d\n", id, r);
    barrier_wait(&barrier);
    printf("Thread %d after barrier, round %d\n", id, r);

    // Optional: simulate work after barrier
    usleep(100000);  // 0.1 second
  }

  return NULL;
}

int main() {
  pthread_t threads[N];
  int ids[N];

  // Initialize barrier for N threads
  barrier_init(&barrier, N);

  // Create threads
  for (int i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, thread_func, &ids[i]);
  }

  // Wait for threads to finish
  for (int i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("All threads completed.\n");
  return 0;
}

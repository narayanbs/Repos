/*
 * Custom Condition Variable Implementation Using Semaphore
 *
 * This example demonstrates how to implement a simple condition variable
 * using a semaphore and a mutex. It also includes a producer-consumer
 * example showing how threads can wait for a condition to become true.
 *
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

/* ----------------------- Condition Variable ----------------------- */

typedef struct {
  sem_t sem;              // Semaphore used to block/wake threads
  pthread_mutex_t mutex;  // Mutex to protect 'count'
  int count;              // Number of threads currently waiting
} cond_t;

/**
 * Initialize the condition variable
 */
void cond_init(cond_t *cdt) {
  sem_init(&cdt->sem, 0, 0);              // Semaphore starts at 0
  pthread_mutex_init(&cdt->mutex, NULL);  // Initialize internal mutex
  cdt->count = 0;                         // No threads waiting initially
}

/**
 * Wait on the condition variable
 *
 * @param cond   Pointer to the condition variable
 * @param mutex  Pointer to the external mutex associated with the shared resource
 */
void cond_wait(cond_t *cond, pthread_mutex_t *mutex) {
  // Increment count of waiting threads (protected by cond->mutex)
  pthread_mutex_lock(&cond->mutex);
  cond->count++;
  pthread_mutex_unlock(&cond->mutex);

  // Release external mutex while waiting
  pthread_mutex_unlock(mutex);

  // Wait on semaphore until a signal occurs
  sem_wait(&cond->sem);

  // Re-acquire the external mutex after waking up
  pthread_mutex_lock(mutex);
}

/**
 * Signal (wake up) one waiting thread
 *
 * @param cond  Pointer to the condition variable
 */
void cond_signal(cond_t *cond) {
  pthread_mutex_lock(&cond->mutex);

  // Only signal if there are threads waiting
  if (cond->count > 0) {
    cond->count--;         // One fewer thread waiting
    sem_post(&cond->sem);  // Wake up one thread
  }

  pthread_mutex_unlock(&cond->mutex);
}

/* ----------------------- Producer-Consumer Example ----------------------- */

int data_ready = 0;                                // Shared resource flag
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  // Mutex for shared resource
cond_t cond;                                       // Our custom condition variable

/**
 * Producer thread function
 */
void *producer(void *arg) {
  sleep(1);  // Simulate work

  pthread_mutex_lock(&lock);
  data_ready = 1;
  printf("Producer: data ready\n");

  // Signal consumer that data is ready
  cond_signal(&cond);

  pthread_mutex_unlock(&lock);
  return NULL;
}

/**
 * Consumer thread function
 */
void *consumer(void *arg) {
  pthread_mutex_lock(&lock);

  // Wait until data is ready
  while (!data_ready) {
    printf("Consumer: waiting for data...\n");
    cond_wait(&cond, &lock);
  }

  printf("Consumer: got the data\n");
  pthread_mutex_unlock(&lock);
  return NULL;
}

/* ----------------------- Main Function ----------------------- */

int main(void) {
  // Initialize the custom condition variable
  cond_init(&cond);

  pthread_t prod, cons;

  // Create consumer and producer threads
  pthread_create(&cons, NULL, consumer, NULL);
  pthread_create(&prod, NULL, producer, NULL);

  // Wait for threads to finish
  pthread_join(prod, NULL);
  pthread_join(cons, NULL);

  return 0;
}

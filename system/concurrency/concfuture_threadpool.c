/*
 * Future implementation in C, with a Thread pool
 */
#include <pthread.h>  // POSIX threads (pthread_* APIs)
#include <stdio.h>    // printf
#include <stdlib.h>   // malloc, free
#include <unistd.h>   // sleep

/* =========================
   Future
   ========================= */

/*
 * A Future represents a task that will be executed
 * by the thread pool and whose result will be
 * available later.
 */
typedef struct Future {
  void *(*task)(void *);  // Function pointer to the task to execute
  void *arg;              // Argument passed to the task
  void *result;           // Result produced by the task

  int completed;  // Flag indicating whether task has finished

  pthread_mutex_t mutex;  // Protects access to completed/result
  pthread_cond_t cond;    // Used to wait for task completion

  struct Future *next;  // Next future in the task queue
} Future;

/* =========================
   Thread Pool
   ========================= */

/*
 * The thread pool manages worker threads
 * and a queue of submitted Futures.
 */
typedef struct {
  pthread_t *threads;  // Array of worker threads
  int num_threads;     // Number of worker threads

  Future *queue_head;  // Head of the task queue
  Future *queue_tail;  // Tail of the task queue

  pthread_mutex_t queue_mutex;  // Protects the task queue
  pthread_cond_t queue_cond;    // Signals when new tasks arrive

  int shutdown;  // Flag to stop workers
} ThreadPool;

/* =========================
   Worker thread
   ========================= */

/*
 * Function executed by each worker thread.
 * Continuously pulls tasks from the queue and runs them.
 */
void *worker_thread(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;  // Get thread pool

  while (1) {
    pthread_mutex_lock(&pool->queue_mutex);

    // Wait until there is work to do or shutdown is requested
    while (!pool->queue_head && !pool->shutdown) {
      pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
    }

    // If shutdown flag is set, exit the loop
    if (pool->shutdown) {
      pthread_mutex_unlock(&pool->queue_mutex);
      break;
    }

    // Remove the first Future from the queue
    Future *f = pool->queue_head;
    pool->queue_head = f->next;

    // If queue is now empty, update tail
    if (!pool->queue_head) pool->queue_tail = NULL;

    pthread_mutex_unlock(&pool->queue_mutex);

    /* Execute task */
    void *res = f->task(f->arg);  // Run the task function

    // Store the result and signal completion
    pthread_mutex_lock(&f->mutex);
    f->result = res;                // Save task result
    f->completed = 1;               // Mark as completed
    pthread_cond_signal(&f->cond);  // Wake threads waiting on future
    pthread_mutex_unlock(&f->mutex);
  }

  return NULL;  // Exit worker thread
}

/* =========================
   Thread pool API
   ========================= */

/*
 * Create and initialize a thread pool with a given
 * number of worker threads.
 */
ThreadPool *threadpool_create(int num_threads) {
  ThreadPool *pool = malloc(sizeof(ThreadPool));

  pool->num_threads = num_threads;
  pool->threads = malloc(sizeof(pthread_t) * num_threads);
  pool->queue_head = NULL;
  pool->queue_tail = NULL;
  pool->shutdown = 0;

  // Initialize queue synchronization primitives
  pthread_mutex_init(&pool->queue_mutex, NULL);
  pthread_cond_init(&pool->queue_cond, NULL);

  // Start worker threads
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&pool->threads[i], NULL, worker_thread, pool);
  }

  return pool;
}

/*
 * Submit a task to the thread pool.
 * Returns a Future that can be waited on.
 */
Future *threadpool_submit(ThreadPool *pool, void *(*task)(void *), void *arg) {
  Future *f = malloc(sizeof(Future));

  f->task = task;    // Task function
  f->arg = arg;      // Task argument
  f->result = NULL;  // No result yet
  f->completed = 0;  // Not completed
  f->next = NULL;    // End of queue

  // Initialize Future synchronization primitives
  pthread_mutex_init(&f->mutex, NULL);
  pthread_cond_init(&f->cond, NULL);

  pthread_mutex_lock(&pool->queue_mutex);

  // Append Future to the queue
  if (pool->queue_tail)
    pool->queue_tail->next = f;
  else
    pool->queue_head = f;

  pool->queue_tail = f;

  // Notify a worker that a task is available
  pthread_cond_signal(&pool->queue_cond);
  pthread_mutex_unlock(&pool->queue_mutex);

  return f;
}

/*
 * Block until the Future completes and return its result.
 */
void *future_get(Future *f) {
  pthread_mutex_lock(&f->mutex);

  // Wait until task finishes
  while (!f->completed) {
    pthread_cond_wait(&f->cond, &f->mutex);
  }

  pthread_mutex_unlock(&f->mutex);
  return f->result;  // Return stored result
}

/*
 * Destroy a Future and release its resources.
 */
void future_destroy(Future *f) {
  pthread_mutex_destroy(&f->mutex);
  pthread_cond_destroy(&f->cond);
  free(f);
}

/*
 * Shut down the thread pool and clean up resources.
 */
void threadpool_destroy(ThreadPool *pool) {
  pthread_mutex_lock(&pool->queue_mutex);
  pool->shutdown = 1;                         // Signal workers to stop
  pthread_cond_broadcast(&pool->queue_cond);  // Wake all workers
  pthread_mutex_unlock(&pool->queue_mutex);

  // Wait for all worker threads to exit
  for (int i = 0; i < pool->num_threads; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  pthread_mutex_destroy(&pool->queue_mutex);
  pthread_cond_destroy(&pool->queue_cond);

  free(pool->threads);
  free(pool);
}

/* =========================
   Example task
   ========================= */

/*
 * A slow task that sleeps for 1 second
 * and returns the square of an integer.
 */
void *slow_task(void *arg) {
  int n = *(int *)arg;  // Read input value
  sleep(1);             // Simulate slow computation

  int *res = malloc(sizeof(int));
  *res = n * n;  // Compute result
  return res;    // Return heap-allocated result
}

/* =========================
   Main
   ========================= */

int main(void) {
  // Create a thread pool with 4 worker threads
  ThreadPool *pool = threadpool_create(4);

  int a = 2, b = 3, c = 4;

  // Submit tasks to the thread pool
  Future *f1 = threadpool_submit(pool, slow_task, &a);
  Future *f2 = threadpool_submit(pool, slow_task, &b);
  Future *f3 = threadpool_submit(pool, slow_task, &c);

  // Wait for results
  int *r1 = future_get(f1);
  int *r2 = future_get(f2);
  int *r3 = future_get(f3);

  // Print results
  printf("Results: %d %d %d\n", *r1, *r2, *r3);

  // Free task results
  free(r1);
  free(r2);
  free(r3);

  // Destroy futures
  future_destroy(f1);
  future_destroy(f2);
  future_destroy(f3);

  // Shut down the thread pool
  threadpool_destroy(pool);
  return 0;
}

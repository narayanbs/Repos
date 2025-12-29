#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* =========================
   Lock-free Queue (MS Queue)
   ========================= */

typedef struct Node {
  void *value;
  _Atomic(struct Node *) next;
} Node;

typedef struct {
  _Atomic(Node *) head;
  _Atomic(Node *) tail;
} LFQueue;

/* Initialize lock-free queue */
void lfqueue_init(LFQueue *q) {
  Node *dummy = malloc(sizeof(Node));
  dummy->value = NULL;
  atomic_store(&dummy->next, NULL);
  atomic_store(&q->head, dummy);
  atomic_store(&q->tail, dummy);
}

/* Enqueue (lock-free) */
void lfqueue_enqueue(LFQueue *q, void *value) {
  Node *node = malloc(sizeof(Node));
  node->value = value;
  atomic_store(&node->next, NULL);

  while (1) {
    Node *tail = atomic_load(&q->tail);
    Node *next = atomic_load(&tail->next);

    if (next == NULL) {
      if (atomic_compare_exchange_weak(&tail->next, &next, node)) {
        atomic_compare_exchange_weak(&q->tail, &tail, node);
        return;
      }
    } else {
      atomic_compare_exchange_weak(&q->tail, &tail, next);
    }
  }
}

/* Dequeue (lock-free) */
void *lfqueue_dequeue(LFQueue *q) {
  while (1) {
    Node *head = atomic_load(&q->head);
    Node *tail = atomic_load(&q->tail);
    Node *next = atomic_load(&head->next);

    if (!next) return NULL;

    if (head == tail) {
      atomic_compare_exchange_weak(&q->tail, &tail, next);
      continue;
    }

    void *value = next->value;
    if (atomic_compare_exchange_weak(&q->head, &head, next)) {
      free(head);
      return value;
    }
  }
}

/* =========================
   Future
   ========================= */

typedef struct {
  void *(*task)(void *);
  void *arg;
  void *result;
  atomic_int completed;
} Future;

/* =========================
   Thread Pool
   ========================= */

typedef struct {
  pthread_t *threads;
  int num_threads;
  LFQueue queue;
  atomic_int shutdown;
} ThreadPool;

/* =========================
   Worker Thread
   ========================= */

void *worker_thread(void *arg) {
  ThreadPool *pool = arg;

  while (!atomic_load(&pool->shutdown)) {
    Future *f = lfqueue_dequeue(&pool->queue);

    if (!f) {
      sched_yield(); /* reduce CPU pressure */
      continue;
    }

    void *res = f->task(f->arg);
    f->result = res;
    atomic_store_explicit(&f->completed, 1, memory_order_release);
  }

  return NULL;
}

/* =========================
   Thread Pool API
   ========================= */

ThreadPool *threadpool_create(int n) {
  ThreadPool *pool = malloc(sizeof(ThreadPool));
  pool->num_threads = n;
  pool->threads = malloc(sizeof(pthread_t) * n);
  atomic_store(&pool->shutdown, 0);

  lfqueue_init(&pool->queue);

  for (int i = 0; i < n; i++) {
    pthread_create(&pool->threads[i], NULL, worker_thread, pool);
  }

  return pool;
}

Future *threadpool_submit(ThreadPool *pool, void *(*task)(void *), void *arg) {
  Future *f = malloc(sizeof(Future));
  f->task = task;
  f->arg = arg;
  f->result = NULL;
  atomic_store(&f->completed, 0);

  lfqueue_enqueue(&pool->queue, f);
  return f;
}

/* Spin-wait future */
void *future_get(Future *f) {
  while (!atomic_load_explicit(&f->completed, memory_order_acquire)) {
    sched_yield();
  }
  return f->result;
}

void future_destroy(Future *f) {
  free(f);
}

void threadpool_destroy(ThreadPool *pool) {
  atomic_store(&pool->shutdown, 1);

  for (int i = 0; i < pool->num_threads; i++) {
    pthread_join(pool->threads[i], NULL);
  }

  free(pool->threads);
  free(pool);
}

/* =========================
   Example Task
   ========================= */

void *slow_task(void *arg) {
  int n = *(int *)arg;
  sleep(1);

  int *res = malloc(sizeof(int));
  *res = n * n;
  return res;
}

/* =========================
   Main
   ========================= */

int main(void) {
  ThreadPool *pool = threadpool_create(4);

  int a = 2, b = 3, c = 4;

  Future *f1 = threadpool_submit(pool, slow_task, &a);
  Future *f2 = threadpool_submit(pool, slow_task, &b);
  Future *f3 = threadpool_submit(pool, slow_task, &c);

  int *r1 = future_get(f1);
  int *r2 = future_get(f2);
  int *r3 = future_get(f3);

  printf("Results: %d %d %d\n", *r1, *r2, *r3);

  free(r1);
  free(r2);
  free(r3);

  future_destroy(f1);
  future_destroy(f2);
  future_destroy(f3);

  threadpool_destroy(pool);
  return 0;
}

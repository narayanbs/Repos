#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* =========================
   Future structure
   ========================= */

typedef struct {
  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  int completed;
  void *result;

  void *(*task)(void *);
  void *arg;
} Future;

/* =========================
   Worker thread
   ========================= */

void *future_worker(void *arg) {
  Future *f = (Future *)arg;

  /* Execute the task */
  void *res = f->task(f->arg);

  /* Store result and notify */
  pthread_mutex_lock(&f->mutex);
  f->result = res;
  f->completed = 1;
  pthread_cond_signal(&f->cond);
  pthread_mutex_unlock(&f->mutex);

  return NULL;
}

/* =========================
   Future API
   ========================= */

Future *future_create(void *(*task)(void *), void *arg) {
  Future *f = malloc(sizeof(Future));
  if (!f) return NULL;

  f->task = task;
  f->arg = arg;
  f->completed = 0;
  f->result = NULL;

  pthread_mutex_init(&f->mutex, NULL);
  pthread_cond_init(&f->cond, NULL);

  pthread_create(&f->thread, NULL, future_worker, f);

  return f;
}

void *future_get(Future *f) {
  pthread_mutex_lock(&f->mutex);
  while (!f->completed) {
    pthread_cond_wait(&f->cond, &f->mutex);
  }
  pthread_mutex_unlock(&f->mutex);
  return f->result;
}

void future_destroy(Future *f) {
  pthread_join(f->thread, NULL);
  pthread_mutex_destroy(&f->mutex);
  pthread_cond_destroy(&f->cond);
  free(f);
}

/* =========================
   Example async task
   ========================= */

void *slow_task(void *arg) {
  (void)arg;
  sleep(2);

  int *value = malloc(sizeof(int));
  *value = 42;
  return value;
}

/* =========================
   Main
   ========================= */

int main(void) {
  Future *f = future_create(slow_task, NULL);

  printf("Doing other work...\n");

  int *result = (int *)future_get(f);
  printf("Future result: %d\n", *result);

  free(result);
  future_destroy(f);

  return 0;
}

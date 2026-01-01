/*
 * ============================================================
 * LightSwitch Pattern – Readers–Writers Problem (C / pthreads)
 * ============================================================
 *
 * This program demonstrates the LightSwitch concurrency pattern.
 *
 * Goal:
 *  - Allow multiple readers to access a shared resource concurrently
 *  - Ensure writers have exclusive access
 *
 * Key Idea (LightSwitch):
 *  - First reader locks the shared resource
 *  - Last reader unlocks the shared resource
 *  - Readers cooperate using a counter protected by a mutex
 *
 * Compilation:
 *   gcc lightswitch.c -pthread -o lightswitch
 *
 * ============================================================
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

/* ------------------------------------------------------------
 * LightSwitch Data Structure
 * ------------------------------------------------------------
 * counter : number of threads currently inside the critical section
 * mutex   : protects access to counter
 */
typedef struct {
  int counter;
  pthread_mutex_t mutex;
} LightSwitch;

/*
 * Initialize the LightSwitch
 */
void lightswitch_init(LightSwitch *ls) {
  ls->counter = 0;
  pthread_mutex_init(&ls->mutex, NULL);
}

/*
 * lightswitch_lock
 * ------------------------------------------------------------
 * Called when a thread wants to ENTER the shared section.
 *
 * If this is the FIRST thread:
 *   - lock the shared semaphore (blocks writers)
 *
 * shared: semaphore protecting the shared resource
 */
void lightswitch_lock(LightSwitch *ls, sem_t *shared) {
  pthread_mutex_lock(&ls->mutex);

  ls->counter++;

  /* First thread locks the shared resource */
  if (ls->counter == 1) {
    sem_wait(shared);
  }

  pthread_mutex_unlock(&ls->mutex);
}

/*
 * lightswitch_unlock
 * ------------------------------------------------------------
 * Called when a thread wants to EXIT the shared section.
 *
 * If this is the LAST thread:
 *   - unlock the shared semaphore (allows writers)
 */
void lightswitch_unlock(LightSwitch *ls, sem_t *shared) {
  pthread_mutex_lock(&ls->mutex);

  ls->counter--;

  /* Last thread releases the shared resource */
  if (ls->counter == 0) {
    sem_post(shared);
  }

  pthread_mutex_unlock(&ls->mutex);
}

/* ------------------------------------------------------------
 * Shared Synchronization Objects
 * ------------------------------------------------------------
 * roomEmpty : ensures writers have exclusive access
 * readSwitch: LightSwitch instance used by readers
 */
sem_t roomEmpty;
LightSwitch readSwitch;

/*
 * Reader Thread Function
 * ------------------------------------------------------------
 * Multiple readers can execute this concurrently.
 */
void *reader(void *arg) {
  int id = *(int *)arg;

  /* Enter reader critical section */
  lightswitch_lock(&readSwitch, &roomEmpty);

  printf("Reader %d is reading\n", id);
  sleep(1);  // simulate reading
  printf("Reader %d finished\n", id);

  /* Exit reader critical section */
  lightswitch_unlock(&readSwitch, &roomEmpty);

  return NULL;
}

/*
 * Writer Thread Function
 * ------------------------------------------------------------
 * Writers require exclusive access.
 */
void *writer(void *arg) {
  int id = *(int *)arg;

  /* Wait until no readers or writers are inside */
  sem_wait(&roomEmpty);

  printf("Writer %d is writing\n", id);
  sleep(2);  // simulate writing
  printf("Writer %d finished\n", id);

  sem_post(&roomEmpty);
  return NULL;
}

/*
 * Main Function
 * ------------------------------------------------------------
 * Creates reader and writer threads and starts execution.
 */
int main() {
  pthread_t r1, r2, w1;
  int id1 = 1, id2 = 2, id3 = 1;

  /* Initialize synchronization primitives */
  sem_init(&roomEmpty, 0, 1);
  lightswitch_init(&readSwitch);

  /* Create threads */
  pthread_create(&r1, NULL, reader, &id1);
  pthread_create(&r2, NULL, reader, &id2);
  pthread_create(&w1, NULL, writer, &id3);

  /* Wait for all threads to finish */
  pthread_join(r1, NULL);
  pthread_join(r2, NULL);
  pthread_join(w1, NULL);

  /* Cleanup */
  sem_destroy(&roomEmpty);
  pthread_mutex_destroy(&readSwitch.mutex);

  return 0;
}

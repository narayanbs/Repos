/*
For signaling, the semaphore is initialized to 0; for mutual exclusion,
the initial value is 1; for multiplexing, the initial value is a positive
number greater than 1. To summarize, the general practice is that the initial
value of the semaphore is the desired number of initial allowed concurrent
accesses.

C library functions – <semaphore.h>

int sem_wait (sem_t *sem);
Decrement the semaphore’s value; block if the value is currently 0.

int sem_post (sem_t *sem);
Increment the semaphore’s value; resume another process if the value is 0.
*/
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>

#define MAXLENGTH 40

struct args {
  sem_t *semaphore;
  char *buffer;
};

void *keyboard_listener(void *_args) {
  struct args *args = (struct args *)_args;
  printf("Enter your name here:");
  assert(fgets(args->buffer, MAXLENGTH, stdin) != NULL);

  sem_post(args->semaphore);
  pthread_exit(NULL);
}

void *keyboard_echo(void *_args) {
  struct args *args = (struct args *)_args;

  sem_wait(args->semaphore);

  char *newline = strchr(args->buffer, '\n');
  if (newline != NULL) *newline = '\0';

  printf("Hello, %s\n", args->buffer);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t threads[2];
  char buffer[MAXLENGTH + 1];
  memset(buffer, 0, sizeof(buffer));

  // in linux the semaphore is stored in /dev/shm/OpenCSF_Sema
  sem_t *sem =
      sem_open("/OpenCSF_Sema", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
  assert(sem != SEM_FAILED);

  struct args args = {sem, buffer};
  assert(pthread_create(&threads[0], NULL, keyboard_listener, &args) == 0);
  assert(pthread_create(&threads[1], NULL, keyboard_echo, &args) == 0);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  sem_unlink("/OpenCSF_Sema");
  return 0;
}

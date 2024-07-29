/*
C library functions – <pthread.h>

int pthread_cond_init (pthread_cond_t *cond, const pthread_condattr_t *attr);
Initialize a condition variable.

int pthread_cond_wait (pthread_cond_t *cond, pthread_mutex_t *mutex);
Release a mutex, wait on a condition, then re-acquire the mutex.

int pthread_cond_signal (pthread_cond_t *cond);
Send a signal to a waiting thread.

int pthread_cond_broadcast (pthread_cond_t *cond);
Send a signal to all waiting threads.

int pthread_cond_destroy (pthread_cond_t *cond);
Delete a condition variable and clean up its associated resources.

*/
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLENGTH 100

struct args {
  int counter;
  pthread_cond_t *input_cond;
  pthread_cond_t *input_processed_cond;
  pthread_mutex_t *mutex;
  char *buffer;
  long current_value;
  bool shutdown;
  bool input_is_string;
};

void flush_buffer(char *buffer) {
  int chr;
  char *newline = strchr(buffer, '\n');
  if (newline == NULL) {
    while (((chr = getchar()) != '\n') && !feof(stdin) && !ferror(stdin))
      ;
  }
}

void *keyboard_listener(void *_args) {
  struct args *args = (struct args *)_args;

  pthread_mutex_lock(args->mutex);
  while (1) {
    printf("Enter a string/number (or shutdown to quit): ");
    memset(args->buffer, 0, MAXLENGTH + 1);
    if (fgets(args->buffer, MAXLENGTH, stdin) == NULL) {
      break;
    }

    long guess = strtol(args->buffer, NULL, 10);
    if (guess != 0) {
      args->current_value = guess;
      args->input_is_string = false;
    } else if (strncmp(args->buffer, "shutdown", 8) != 0) {
      args->input_is_string = true;
    } else
      break;

    pthread_cond_broadcast(args->input_cond);
    pthread_cond_wait(args->input_processed_cond, args->mutex);
    flush_buffer(args->buffer);
  }
  args->shutdown = true;
  pthread_cond_broadcast(args->input_cond);
  pthread_mutex_unlock(args->mutex);
  pthread_exit(NULL);
}

void *count_chars(void *_args) {
  struct args *args = (struct args *)_args;
  /* With condvars, always acquire the lock before waiting */
  pthread_mutex_lock(args->mutex);
  while (1) {
    /* Wait on the input condition */
    pthread_cond_wait(args->input_cond, args->mutex);
    /* Signal received, mutex has been re-acquired */

    /* If the input was shutdown signal, quit */
    if (args->shutdown) break;

    /* If the input was a number, ignore this signal */
    if (!args->input_is_string) continue;

    /* Input was string not shutdown; get length without \n */
    char *newline = strchr(args->buffer, '\n');
    if (newline != NULL) *newline = '\0';
    args->counter += strlen(args->buffer);

    /* Restore the newline for buffer flushing */
    if (newline != NULL) *newline = '\n';

    /* Signal back to keyboard listener that input processing
       is done */
    pthread_cond_signal(args->input_processed_cond);
    /* Do NOT unlock mutex; pthread_cond_wait() will do that */
  }

  /* Shutting down. Send acknowledgement signal back */
  pthread_cond_signal(args->input_processed_cond);
  pthread_mutex_unlock(args->mutex);
  pthread_exit(NULL);
}

void *add_number(void *_args) {
  struct args *args = (struct args *)_args;
  /* With condvars, always acquire the lock before waiting */
  pthread_mutex_lock(args->mutex);
  while (1) {
    /* Wait on the input condition */
    pthread_cond_wait(args->input_cond, args->mutex);
    /* Signal received, mutex has been re-acquired */

    /* If the input was shutdown signal, quit */
    if (args->shutdown) break;

    /* If the input was a string, ignore this signal */
    if (args->input_is_string) continue;

    /* Add current value to the counter and send signal back */
    args->counter += args->current_value;
    pthread_cond_signal(args->input_processed_cond);
    /* Do NOT unlock mutex; pthread_cond_wait() will do that */
  }

  /* Shutting down. Send acknowledgement signal back */
  pthread_cond_signal(args->input_processed_cond);
  pthread_mutex_unlock(args->mutex);
  pthread_exit(NULL);
}

int main(void) {
  pthread_t threads[3];
  pthread_cond_t input_cond;
  pthread_cond_t input_processed_cond;
  pthread_mutex_t mutex;
  char buffer[MAXLENGTH + 1];

  /* Initialize the mutex and the condition variables */
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&input_cond, NULL);
  pthread_cond_init(&input_processed_cond, NULL);

  /* Set up the args for all threads */
  struct args args;
  args.counter = 0;
  args.input_cond = &input_cond;
  args.input_processed_cond = &input_processed_cond;
  args.mutex = &mutex;
  args.buffer = buffer;
  args.current_value = 0;
  args.shutdown = false;
  args.input_is_string = false;

  /* Create and join the threads */
  assert(pthread_create(&threads[0], NULL, count_chars, &args) == 0);
  assert(pthread_create(&threads[1], NULL, add_number, &args) == 0);
  assert(pthread_create(&threads[2], NULL, keyboard_listener, &args) == 0);
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);

  /* Print out total, destroy the mutex and condition variables */
  printf("Total: %d\n", args.counter);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&input_cond);
  pthread_cond_destroy(&input_processed_cond);
}

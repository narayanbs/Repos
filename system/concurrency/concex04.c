/* Program to capture a return value from thread using pthread_join */
#include <pthread.h>
#include <stdio.h>

void *thread_function(void *arg) {
  int *num = (int *)arg;
  int result = (*num) * 20;
  pthread_exit((void *)(long)result);
}

int main(void) {
  pthread_t t1;
  int num = 3;
  void *retval;

  if (pthread_create(&t1, NULL, thread_function, &num) != 0) {
    perror("failed to create thread");
    return 1;
  }

  if (pthread_join(t1, &retval) != 0) {
    perror("Failed to join thread");
    return 1;
  }

  int result = (int)(long)retval;
  printf("Thread returned: %d\n", result);
  return 0;
}

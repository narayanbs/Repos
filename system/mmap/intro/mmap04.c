/*
 * IPC
 */
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int N = 5;
  int *ptr = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANON, 0, 0);
  if (ptr == MAP_FAILED) {
    printf("Mapping failed");
    return 1;
  }

  for (int i = 0; i < N; i++) {
    ptr[i] = i + 1;
  }

  printf("Initial values of the array elements: \n");
  for (int i = 0; i < N; i++) {
    printf(" %d", ptr[i]);
  }
  printf("\n");

  pid_t child_pid = fork();
  if (child_pid == 0) {
    for (int i = 0; i < N; i++) {
      ptr[i] = ptr[i] * 10;
    }
  } else {
    waitpid(child_pid, NULL, 0);
    printf("\nParent:\n");
    printf("Updated values of the array elements: \n");
    for (int i = 0; i < N; i++) {
      printf(" %d", ptr[i]);
    }
    printf("\n");
  }

  int err = munmap(ptr, N * sizeof(int));
  if (err != 0) {
    perror("Unmapping failed");
    return 1;
  }
  return 0;
}

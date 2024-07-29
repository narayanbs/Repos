/*
 * Memory allocation example
 */

#include <stdio.h>
#include <sys/mman.h>
int main(void) {
  int N = 5;
  int *ptr = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE,
                  MAP_ANON | MAP_PRIVATE, 0, 0);
  if (ptr == MAP_FAILED) {
    printf("Mapping failed\n");
    return 1;
  }

  for (int i = 0; i < N; i++) {
    ptr[i] = i * 10;
  }

  printf("The elements of the array => ");
  for (int i = 0; i < N; i++) {
    printf("[%d] ", ptr[i]);
  }
  printf("\n");
  int err = munmap(ptr, N * sizeof(int));
  if (err != 0) {
    printf("Unamapping failed\n");
    return 1;
  }
  return 0;
}

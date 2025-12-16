/*
 * The posix_memalign function allows you to allocate memory with a
 * specific alignment. It provides a way to ensure that the allocated memory is
 * aligned to a boundary that is a multiple of a specified alignment.
 *
 * Note: Alignment must be specified as a power of 2 and >= sizeof(void *)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  void *ptr;
  size_t alignment = 16;  // must be a power of 2
  size_t memory_size = 1024;

  if (posix_memalign(&ptr, alignment, memory_size) != 0) {
    perror("posix_memalign failed");
    return 1;
  }

  // use ptr as needed
  printf("Memory allocated at address: %p\n", ptr);
  if (((uintptr_t)ptr) % alignment == 0) {
    printf("Memory is aligned at %zu\n", alignment);
  } else {
    printf("Memory is not aligned at %zu\n", alignment);
  }
  free(ptr);
  return 0;
}

/*
 * The aligned_alloc function in C11 allows you to allocate memory with a
 * specific alignment. It provides a way to ensure that the allocated memory is
 * aligned to a boundary that is a multiple of a specified alignment.
 *
 * Note: Alignment must be specified as a power of 2 and >= sizeof(void *)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Define the alignment and size of the memory block

  // Alignment must be a power of 2 and >= sizeof(void *)
  size_t alignment = 64;
  size_t size = 1024;  // Size of the memory block to allocate

  // Allocate memory with the specified alignment
  void *ptr = aligned_alloc(alignment, size);

  if (ptr == NULL) {
    // Handle allocation failure
    perror("aligned_alloc failed");
    return EXIT_FAILURE;
  }

  // Print the address of the allocated memory
  printf("Allocated memory address: %p\n", ptr);

  // Ensure the memory is aligned
  if ((uintptr_t)ptr % alignment == 0) {
    printf("Memory is correctly aligned.\n");
  } else {
    printf("Memory is not aligned.\n");
  }

  // Use the allocated memory...

  // Free the allocated memory
  free(ptr);

  return EXIT_SUCCESS;
}

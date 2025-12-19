/*
 * alloc, calloc and realloc
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  // allocated memory is uninitialized
  int *ptr = malloc(4 * sizeof(int));
  free(ptr);

  // allocated memory is zero initialized
  ptr = calloc(4, sizeof(int));

  // realloc involves 3 cases
  // if there isn't sufficient space it returns NULL
  // and leaves the previous region as it is (allocated)
  // if there is sufficient space
  // then it may point to the same memory
  // or it may point to a different region while
  // destroying the original buffer
  int *ptr1 = realloc(ptr, 10 * sizeof(int));
  if (ptr1 == NULL) {
    printf("Exiting\n");
    free(ptr);
    exit(0);
  } else {
    ptr = ptr1;
  }
  return 0;
}

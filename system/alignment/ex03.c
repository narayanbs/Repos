/*
 * Example to indicate misaligned access using pointers
 *
 *
 * Read more at Articles/Pointer_Aligned_access.html
 */
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * undefined behavior, q is misaligned
 */
int misaligned_func1(int* p, int* q) {
  *p = 1;
  *q = 1;
  return *p;
}

/*
 * memcpy to misaligned address is also undefined behavior
 */
int misaligned_func2(int* p, int* q) {
  *p = 1;
  int one = 1;
  memcpy(q, &one, sizeof *q);
  return *p;
}

void f(void) {
  char* t = malloc(1 + sizeof(int));
  if (!t) abort();
  int* fp = (int*)t;
  int* fq = (int*)(t + 1);

  int r = misaligned_func1(fp, fq);
  printf("%d %d\n", r, *fp);
  assert(r == *fp);

  // same as above.
  // int r = misalgined_func2(fp, fq);
}

int main(void) {
  int* p = (int*)malloc(sizeof(int));
  if (p == NULL) {
    perror("Failed to allocate memory\n");
    return 1;
  }
  printf("Address of p: %p\n", (void*)p);
  free(p);

  // check alignment of pointer
  if ((uintptr_t)p % alignof(int) == 0) {
    printf("Pointer is correctly aligned for int.\n");
  } else {
    printf("Pointer is not correctly aligned for int.\n");
  }

  // function to create and access misaligned pointers
  f();
  return 0;
}

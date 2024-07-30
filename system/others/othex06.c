/*
 * Example to indicate misaligned access using pointers
 * function h is optiimized by gcc to return 1,
 * even though f is passing misaligned pointers
 *
 * function solutionh is a way to return the correct answer
 * even if misaligned pointers are passed.
 *
 * Note: memcpy doesn't care about endianess while copying it just
 * copies bytes as it is. 
 *
 * Read more at Articles/Pointer_Aligned_access.html
 *
 */ 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int h(int* p, int* q) {
  *p = 1;
  *q = 1;
  return *p;

}

int solutionh(int* p, int* q) {
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
  int r = h(fp, fq);
  printf("%d %d\n", r, *fp);
  assert(r == *fp);
}

int main(void) {
  f();
  return 0;
}

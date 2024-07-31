/*
 * If a pointer is known to be correctly aligned to the target type, then a cast to that type is
 * permitted. There are several cases where a pointer is known to be correctly aligned to the target
 * type. The pointer could point to an object declared with a suitable alignment specifier. It could
 * point to an object returned by aligned_alloc(), calloc(), malloc(), or realloc(), as per the C
 * standard, section 7.22.3, paragraph 1  [ISO/IEC 9899:2011]. This compliant solution uses the
 * alignment specifier, which is new to C11, to declare the char object c with the same alignment as
 * that of an object of type int. As a result, the two pointers reference equally aligned pointer
 * types:
 */

#include <assert.h>
#include <stdalign.h>

void func(void) {
  // Align c to the alignment of an int
  // Here c is stored in an address which meets
  // the alignment requirement of int
  alignas(int) char c = 'x';
  // then you can cast the pointer to c to a pointer to int and back.
  // this is valid in c. but dereferencing ip could cause problems
  int *ip = (int *)&c;
  char *cp = (char *)ip;
  assert(cp == &c);
}

int main(void) { func(); }

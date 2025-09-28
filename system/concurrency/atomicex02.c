/*
 * Atomic fetch and add
 */
#include <stdatomic.h>
#include <stdio.h>

int main(void) {
  atomic_int counter = 20;
  atomic_fetch_add(&counter, 5);
  printf("Counter value after atomic addition: %d\n", atomic_load(&counter));
  return 0;
}

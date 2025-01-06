/*
 * Atomic load and store
 */
#include <stdatomic.h>
#include <stdio.h>

int main(void) {
  atomic_int atomicvar = ATOMIC_VAR_INIT(10);
  atomic_store(&atomicvar, 20);
  int value = atomic_load(&atomicvar);
  printf("The atomic variable's value is %d\n", value);
  return 0;
}

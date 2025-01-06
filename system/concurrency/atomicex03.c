/*
 * Atomic compare and exchange
 */
#include <stdatomic.h>
#include <stdio.h>

int main(void) {
  atomic_int atomicvar = ATOMIC_VAR_INIT(100);
  int expected = 100;
  int desired = 200;

  if (atomic_compare_exchange_strong(&atomicvar, &expected, desired)) {
    printf("Value successfully changed to %d\n", atomic_load(&atomicvar));
  } else {
    printf("Value change failed. Current value is %d\n", expected);
  }
  return 0;
}

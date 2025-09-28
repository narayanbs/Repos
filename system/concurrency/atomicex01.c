/*
 * Atomic initialization
 * Atomic load and store
 */
#include <stdatomic.h>
#include <stdio.h>

int main(void) {
  // static initialization
  // Deprecated Macro initialization
  // atomic_int atomicvar = ATOMIC_VAR_INIT(10);
  atomic_int i = 50;
  printf("The atomic variable i's value is %d\n", i);

  atomic_long l = 100;
  printf("The atomic variable l's value is %ld\n", l);

  // dynamic initialization
  atomic_int x;
  atomic_init(&x, 42);
  printf("The atomic variable x's value is %d\n", atomic_load(&x));

  atomic_long y;
  atomic_init(&y, 2234);
  printf("The atomic variable y's value is %ld\n", atomic_load(&y));

  // atomic flog can only be initialized by macro
  // it cannot be initialized by atomic_init
  atomic_flag lock = ATOMIC_FLAG_INIT;
  if (!atomic_flag_test_and_set(&lock)) {
    printf("Flag was clear. Now set it \n");
  } else {
    printf("Flag was already set\n");
  }
  atomic_flag_clear(&lock);

  atomic_store(&x, 20);
  int value = atomic_load(&x);
  printf("The atomic variable x's value is %d\n", value);
  return 0;
}

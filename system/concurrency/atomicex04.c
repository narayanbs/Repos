/*
 * modifying a struct atomically in C language
 * But Here's the Catch:
 * Atomicity depends on the size and alignment of the struct.
 * The compiler and hardware must support atomic operations on objects of that size.
 * If the struct is too large or not properly aligned, atomic operations might not be lock-free,
 * or might not even be supported.
 *
 * You cannot do atomic operations on individual members of the struct via the atomic struct.
 * for ex: p.x = 20;
 *
 * if you want to modify individual members atomically, you have to make each member atomic:
 * typedef struct {
 *   atomic_int x;
 *   atomic_int y;
 * } AtomicPoint;
 *
 * Then you can do
 * atomic_store(&point.x, 5); // ✅ atomic
 *
 */
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  int x;
  int y;
} Point;

int main() {
  _Atomic Point p;

  if (atomic_is_lock_free(&p)) {
    printf("Atomic operations on Point are lock free\n");
  } else {
    printf("Atomic operations on Point are not lock free\n");
    return 1;
  }

  Point a = {1, 2};
  atomic_store(&p, a);  // atomic store

  Point b = atomic_load(&p);  // atomic load

  printf("struct Point b after load {%d, %d}\n", b.x, b.y);

  Point new_value = {3, 4};

  // Point old = atomic_exchange(&p, (Point){3, 4});
  Point old = atomic_exchange(&p, new_value);

  Point expected = {1, 2};
  Point provided = {5, 6};
  bool swapped = atomic_compare_exchange_strong(&p, &expected, provided);

  return 0;
}

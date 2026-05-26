/*
 * We can terminate a program using exit.
 * The argument to exit returns a status code to the operating system.
 *
 * What exit() does before termination:
 * Calls all atexit() handlers (in reverse registration order)
 * Flushes and closes open streams/files
 * Returns control to the operating system
 *
 * In the following program the o/p will be
 * Inside main function
 * Cleanup function 2 executed
 * Cleanup function 1 executed
 *
 * The code after exit wont be executed.
 *
 * Note: return statement inside main function also returns a status code to the OS.
 * so return 0 is similar to exit(0). (Of course, the difference is that exit invokes handlers as
 * mentioned above)
 */
#include <stdio.h>
#include <stdlib.h>

void cleanup1() {
  printf("Cleanup function 1 executed\n");
}

void cleanup2() {
  printf("Cleanup function 2 executed\n");
}

int main() {
  // Register cleanup functions
  atexit(cleanup1);
  atexit(cleanup2);

  printf("Inside main function\n");

  // Terminate program
  exit(0);

  // This line never executes
  printf("End of program\n");

  return 0;
}

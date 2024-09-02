/* PRNG in c language to generate random numbers
 * Two primary functions rand() and srand()
 * rand() generates a random number between [0, RAND_MAX]
 * srand() sets the seed, if srand is not called then rand seet is
 * set as if srand(1) is called at program start.
 */
// Simple example of random number generation

#include <stdio.h>
#include <stdlib.h>  // for srand(), rand()
#include <time.h>    // for time()

int main() {
  srand(time(0));  // seed the random number generator
  int r;

  float f;

  r = rand();
  printf("The random number chosen (no range specified) is: %d\n", r);

  r = rand() % 10;
  printf("A random number in the range (0-9):  %d\n", r);

  r = rand() % 50 + 51;
  printf("A random number in the range (51-100):  %d\n", r);

  f = (float)rand() / (float)RAND_MAX;
  printf("A float number in the range (0 - 1): %f\n", f);

  float low = 5, high = 10;
  f = ((float)rand() / (float)RAND_MAX) * (high - low) + low;
  printf("A float number in the range (5 - 10): %f\n", f);

  return 0;
}

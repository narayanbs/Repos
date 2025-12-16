/*
 * Multi dimensional arrays
 */
#include <stdio.h>
#include <stdlib.h>

#define ROWS 3
#define COLS 3

int main(void) {
  int a[ROWS][COLS];
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      a[i][j] = (i + j) * 10;
    }
  }

  // multi dimensional array as pointer
  int(*p)[COLS] = a;  // or &a[0][0];
  printf("%d\n", p[2][2]);

  // int **p = a; // error

  // dynamic allocation
  // as pointer
  int *x = malloc(ROWS * COLS * sizeof(int));
  // x[i * COLS + j] = 43;
  x[1 * COLS + 2] = 43;

  printf("%d\n", *(x + (1 * COLS + 2)));

  // as pointer to array
  int(*y)[COLS] = malloc(ROWS * COLS * sizeof(int));
  // int(*y)[COLS] = malloc(ROWS * sizeof(*y));

  y[1][2] = 43;
  printf("%d\n", y[1][2]);
  return 0;
}

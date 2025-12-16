
/*
 * Three dimensional arrays
 *
 * int arr[X][Y][Z]
 * &a[i][j][k] = base_address + ((i * Y + j) * Z + k)  * sizeof(int)
 */

#include <stdio.h>

#define max_lines 10
#define line_length 40

#define X 3
#define Y 3
#define Z 3

int main(void) {
  int a[X][Y][Z];
  for (int i = 0; i < X; i++)
    for (int j = 0; j < Y; j++)
      for (int k = 0; k < Z; k++) {
        a[i][j][k] = (i + j + k) * 10;
      }

  int *ptr = &a[0][0][0];

  printf("%d\n", a[1][2][1]);
  printf("%d \n", *(ptr + ((1 * Y + 2) * Z + 1)));
}

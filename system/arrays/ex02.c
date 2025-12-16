
/*
 * Three dimensional arrays
 *
 * Flattening formula
 * int arr[D][R][C], where D is depth, R -> Rows and C -> Columns
 * To access an element at a[d][r][c] (where d,r,c are depth, row and column index respectively)
 *
 * &a[d][r][c] = a[d * (R * C) + r * C + c]
 */
#include <stdio.h>

#define DEPTH 2  // D
#define ROWS 3   // R
#define COLS 4   // C

int main() {
  int array3D[DEPTH][ROWS][COLS];

  // --- 1. Populate the 3D array (for demonstration) ---
  int count = 0;
  for (int d = 0; d < DEPTH; d++) {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        // Assign a unique value based on its 1D position (i)
        array3D[d][r][c] = count++;
      }
    }
  }

  // --- 2. Calculate the 1D index for a specific element ---
  // Let's find the element at indices [1][2][3]
  int target_d = 1;
  int target_r = 2;
  int target_c = 3;

  // The Flattening Calculation: i = d * (R * C) + r * C + c
  int flat_index = target_d * (ROWS * COLS) + target_r * COLS + target_c;

  printf("The element at array3D[%d][%d][%d] is: %d\n", target_d, target_r, target_c,
         array3D[target_d][target_r][target_c]);

  printf("Its calculated 1D (flat) index is: %d\n", flat_index);

  // --- 3. Access the element using its calculated 1D index ---
  // In C, the name of a multi-dimensional array decays to a pointer to its first element.
  // We can cast the 3D array to a 1D pointer:
  int *array1D = (int *)array3D;

  int element_from_1D = array1D[flat_index];

  printf("The element accessed via the 1D pointer at index %d is: %d\n", flat_index,
         element_from_1D);

  return 0;
}

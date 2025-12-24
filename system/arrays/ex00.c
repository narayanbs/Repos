#include <stdio.h>

void printsingarr(int *arr, int N) {
  for (int i = 0; i < N; i++) {
    printf("%d ", *(arr + i));
  }
  printf("\n");
}

void printdoubarr(int (*arr)[3], int R, int C) {
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      printf("%d ", *(*(arr + i) + j));
    }
    printf("\n");
  }
}

void printdoubarrsingptr(int *ptr, int R, int C) {
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      printf("%d ", *(ptr + i * C + j));
    }
    printf("\n");
  }
}

void printtriparr(int (*arr)[2][2], int D, int R, int C) {
  for (int i = 0; i < D; i++) {
    for (int j = 0; j < R; j++) {
      for (int k = 0; k < C; k++) {
        printf("%d ", arr[i][j][k]);
      }
      printf("\n");
    }
  }
}

void printtriparrsingptr(int *ptr, int D, int R, int C) {
  for (int i = 0; i < D; i++) {
    for (int j = 0; j < R; j++) {
      for (int k = 0; k < C; k++) {
        printf("%d ", *(ptr + (i * R + j) * C + k));
      }
      printf("\n");
    }
  }
  printf("\n");
}

int main(void) {
  int arr[] = {1, 2, 3, 4, 5};
  printsingarr(arr, 5);

  printf("--------------\n");

  int brr[2][3] = {{1, 2, 3}, {4, 5, 6}};
  printdoubarr(brr, 2, 3);
  printdoubarrsingptr(brr, 2, 3);

  printf("--------------\n");

  int crr[2][2][2] = {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
  printtriparr(crr, 2, 2, 2);
  printtriparrsingptr(crr, 2, 2, 2);
}

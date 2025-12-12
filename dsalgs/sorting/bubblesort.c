/*
 * Bubble Sort
 */
#include <stdio.h>

void sort(int arr[], int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n - 1 - i; j++) {
      if (arr[j] > arr[j + 1]) {
        int tmp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = tmp;
      }
    }
  }
}

void printarr(int arr[], int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

int main() {
  int arr[] = {40, 10, 56, 2, 1, 4, 19, 3};
  int n = sizeof(arr) / sizeof(arr[0]);
  printf("Array before sorting ");
  printarr(arr, n);
  sort(arr, n);
  printf("Array after sorting ");
  printarr(arr, n);
}

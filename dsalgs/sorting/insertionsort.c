/*
 * Insertion sort
 */
#include <stdio.h>

void sort(int arr[], int n) {
  for (int i = 1; i < n; i++) {
    int j = i - 1;
    int key = arr[i];
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    j++;
    arr[j] = key;
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

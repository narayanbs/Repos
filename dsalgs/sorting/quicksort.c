/*
 * Quick sort
 */
#include <stdio.h>

void swap(int *a, int *b);

int partition(int arr[], int l, int r) {
  int pivot = arr[r];
  int i = l - 1;

  for (int j = l; j <= r - 1; j++) {
    if (arr[j] < pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[r]);
  return i + 1;
}

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void quicksort(int arr[], int l, int r) {
  if (l < r) {
    int pi = partition(arr, l, r);
    quicksort(arr, l, pi - 1);
    quicksort(arr, pi + 1, r);
  }
}

void sort(int arr[], int n) {
  quicksort(arr, 0, n - 1);
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

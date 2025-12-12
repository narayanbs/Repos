/*
 * MergeSort
 */
#include <stdio.h>

void merge(int arr[], int l, int mid, int r) {
  int n1 = mid - l + 1;
  int n2 = r - mid;
  int L[n1], R[n2];
  for (int i = 0; i < n1; i++) L[i] = arr[l + i];
  for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

  int i = 0, j = 0;
  int k = l;

  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

void mergesort(int arr[], int l, int r) {
  if (l < r) {
    int mid = l + (r - l) / 2;
    mergesort(arr, l, mid);
    mergesort(arr, mid + 1, r);
    merge(arr, l, mid, r);
  }
}

void sort(int arr[], int n) {
  mergesort(arr, 0, n - 1);
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

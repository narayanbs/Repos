/*
 * Binary search works well for monotonic arrays (strictly increasing or
 * strictly decreasing),
 * Recursive version
 */
#include <stdio.h>

int binsearch(int arr[], int l, int r, int x) {
  while (l <= r) {
    int mid = l + (r - l) / 2;
    if (arr[mid] == x) {
      return mid;
    }
    if (arr[mid] > x) {
      r = mid - 1;
    } else {
      l = mid + 1;
    }
  }
  return -1;
}

int main() {
  int arr[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
  int n = sizeof(arr) / sizeof(arr[0]);
  int query = 23;
  int result = binsearch(arr, 0, n - 1, query);
  (result == -1) ? printf("Element %d is not present\n", query)
                 : printf("Element %d is present at index %d\n", query, result);
  return 0;
}

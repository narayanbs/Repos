/*
 * Binary search works well for monotonic arrays (strictly increasing or
 * strictly decreasing),
 * Recursive version
 *
 * Note:
 * The formula mid = l + (r - l) / 2
 * is the safe way to compute the midpoint between two integers l (left) and r (right), especially
 * in binary search. instead of mid = (l + r) / 2
 * Reason is l + r may overflow if l and r are large, producing a negative number.
 * Example on 32-bit machine INT_MAX  = 2147483647
 * l = 2,000,000,000 r = 2,100,000,000, l + r = 4,100,000,000  // overflows!
 * but mid = l + (r-l) /2 works safely
 * r - l = 100,000,000
 * (r - l) / 2 = 50,000,000
 * mid = l + 50,000,000 = 2,050,000,000  // correct
 *
 */
#include <stdio.h>

int binsearch(int arr[], int l, int r, int x) {
  if (l <= r) {
    int mid = l + (r - l) / 2;
    if (arr[mid] == x) {
      return mid;
    } else if (arr[mid] > x) {
      return binsearch(arr, l, mid - 1, x);
    } else {
      return binsearch(arr, mid + 1, r, x);
    }
  }
  return -1;
}

int main() {
  int arr[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
  int n = sizeof(arr) / sizeof(arr[0]);
  int query[] = {4, 2, 91, 12};
  for (int i = 0; i < 4; i++) {
    int result = binsearch(arr, 0, n - 1, query[i]);
    (result == -1) ? printf("Element %d is not present\n", query[i])
                   : printf("Element %d is present at index %d\n", query[i], result);
  }
  return 0;
}

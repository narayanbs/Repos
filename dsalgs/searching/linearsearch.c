#include <stdio.h>

int search(int arr[], int n, int x) {
  for (int i = 0; i < n; i++)
    if (arr[i] == x)
      return i;
  return -1;
}
int main(void) {
  int arr[] = {2, 3, 4, 10, 40};
  int x = 10;
  int n = sizeof(arr) / sizeof(arr[0]);

  int result = search(arr, n, x);
  (result == -1) ? printf("Element %d is not present\n", x)
                 : printf("Element %d is present at index %d\n", x, result);
  return 0;
}

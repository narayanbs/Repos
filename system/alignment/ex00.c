/*
 * Obtaining alignment of types
 */

#include <stdalign.h>
#include <stdio.h>

int main(void) {
  // Print the alignment requirements of various types
  printf("Alignment of char: %zu bytes\n", alignof(char));
  printf("Alignment of int: %zu bytes\n", alignof(int));
  printf("Alignment of double: %zu bytes\n", alignof(double));
  printf("Alignment of long: %zu bytes\n", alignof(long));
  printf("Alignment of long long: %zu bytes\n", alignof(long long));
  printf("Alignment of struct { char c; int i; }: %zu bytes\n", alignof(struct {
           char c;
           int i;
         }));

  // Define a struct with different types
  struct MyStruct {
    char c;
    int i;
    double d;
  };

  // Print the alignment requirement of the struct
  printf("Alignment of MyStruct { char c; int i; double d; }; : %zu bytes\n",
         alignof(struct MyStruct));

  return 0;
}

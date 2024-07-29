/*
for better error handling, use  strtof, strtod, strtold, strtol, strtoul, strtoull from stdlib.h

strtol ignores whitespaces at the start, and also stores the "rest of the string" in remaining.
*/

#include <stdio.h>
#include <stdlib.h>  // the library required to use strtol

int main() {
  // string to long integer conversion
  char str[50] = "  2020111224234234 rest of the string";
  char* remaining;
  long answer;
  answer = strtol(str, &remaining, 10);
  printf("The converted long integer is %ld\n", answer);

  // string to hexadecimal conversion
  char str1[50] = "0x85fa4b";
  answer = strtol(str1, &remaining, 16);
  printf("The converted hexadecimal is %lx\n", answer);  // %lx is the format for hexadecimals

  char str2[20] = "1.234234";
  float result = strtof(str2, &remaining);
  printf("The converted float is %f\n", result);

  // badcharacter
  char* s = "34t90";
  char* badchar;
  unsigned long int x = strtoul(s, &badchar, 10);
  printf("%lu ", x);                       // 34
  printf("Invalid char: %c\n", *badchar);  // t
  return 0;
}

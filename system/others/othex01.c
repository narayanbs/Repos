/*
 C program to get the length of a string with multibyte characters
*/
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int main(void) {
  setlocale(LC_ALL, "");

  char* mb_string = "\u20ac1.23";
  size_t mb_len = strlen(mb_string);

  wchar_t wc_string[128];
  size_t wc_len = mbstowcs(wc_string, mb_string, 128);

  printf("multibyte: %s (%zu bytes)\n", mb_string, mb_len);
  printf("wide char: %ls (%zu characters)\n", wc_string, wc_len);
  printf("Length of wide string: %ls is %zu\n", wc_string, wcslen(wc_string));

  // Another way to get the length of a multibyte string is
  // The following string has 7 characters
  size_t len_in_chars = mbstowcs(NULL, "§¶°±π€•", 0);
  printf("Length of multibyte string is %zu\n", len_in_chars);  // 7

  // Again, that’s a non-portable POSIX extension.
  // And, of course, if you want to convert the other way, it’s wcstombs().
  return 0;
}

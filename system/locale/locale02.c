/*
 * simple custom locale-sensitive currency printing function in C.
 */
#include <locale.h>
#include <stdio.h>
#include <string.h>

void print_currency(double amount) {
  struct lconv *lc = localeconv();

  char buffer[100];

  // Format with 2 decimal places
  sprintf(buffer, "%.2f", amount);

  // Replace decimal point if locale uses something else
  char *dot = strchr(buffer, '.');

  if (dot && strcmp(lc->decimal_point, ".") != 0) {
    *dot = lc->decimal_point[0];
  }

  // Print currency symbol + formatted number
  printf("%s%s\n", lc->currency_symbol, buffer);
}

int main() {
  // Use environment locale
  setlocale(LC_ALL, "");

  print_currency(1234.56);

  return 0;
}

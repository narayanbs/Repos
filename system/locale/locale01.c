/*
 * simple custom locale-sensitive function in C that formats a
 * floating-point number using the locale’s decimal separator.
 *
 *
 *  By default C program runs in "C" locale. We need to explicitly
 *  change it if we need to use a different locale.
 *
 *  setLocale(LC_ALL, "") says that
 *
 *  use the locale specified by user's environment
 *  variables like
 *  LC_ALL
 *  LC_CTYPE
 *  LC_TIME
 *  LC_NUMERIC
 *  LANG
 *
 */
#include <locale.h>
#include <stdio.h>
#include <string.h>

void print_localized_number(double value) {
  struct lconv *lc = localeconv();

  char buffer[100];

  // Convert number using standard formatting
  sprintf(buffer, "%.2f", value);

  // Replace '.' with locale decimal separator
  char *dot = strchr(buffer, '.');

  if (dot && strcmp(lc->decimal_point, ".") != 0) {
    *dot = lc->decimal_point[0];
  }

  printf("Localized number: %s\n", buffer);
}

int main() {
  // Use user's environment locale
  setlocale(LC_ALL, "");

  print_localized_number(1234.56);

  return 0;
}

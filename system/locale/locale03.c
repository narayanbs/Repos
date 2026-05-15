/*
 * Locale sensitive date formatting program
 */
#include <locale.h>
#include <stdio.h>
#include <time.h>

void print_localized_date() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  char buffer[100];

  // Locale-aware formatting
  strftime(buffer, sizeof(buffer), "%A, %d %B %Y", t);

  printf("Date: %s\n", buffer);
}

int main() {
  // Use environment locale
  setlocale(LC_ALL, "");

  print_localized_date();

  return 0;
}

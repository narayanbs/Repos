/*
 * Naive algorithm
 * Here we check the match by sliding the pattern one step at a time
 */

#include <cstring>
#include <iostream>

int main() {
  std::string s = "hello how are you doing? are you ok ? are you";
  std::string p = "you";

  int m = s.size();
  int n = p.size();

  for (int i = 0; i < m - n + 1; i++) {
    int equal = 1;
    for (int j = 0; j < n; j++) {
      if (p[j] != s[i + j]) {
        equal = 0;
        break;
      }
    }
    if (equal)
      std::cout << "Pattern found at " << i << std::endl;
  }

  return 0;
}

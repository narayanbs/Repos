/*
 * Boyer Moore - Bad Heuristic
 */
#include <algorithm>
#include <iostream>

#define NUMBER_OF_CHARS 256

void badCharHeuristic(std::string str, int size, int badchar[NUMBER_OF_CHARS]) {
  for (int i = 0; i < NUMBER_OF_CHARS; ++i) {
    badchar[i] = -1;
  }

  for (int i = 0; i < NUMBER_OF_CHARS; ++i) {
    badchar[(int)str[i]] = i;
  }
}

void search(std::string txt, std::string pat) {
  int m = pat.size();
  int n = txt.size();

  int badchar[NUMBER_OF_CHARS];

  badCharHeuristic(pat, m, badchar);

  int shift = 0;
  while (shift <= (n - m)) {
    int j = m - 1;

    while (j >= 0 && pat[j] == txt[shift + j])
      j--;

    if (j < 0) {
      std::cout << "Patter occurs at shift = " << shift << std::endl;

      shift += (shift + m < n) ? m - badchar[txt[shift + m]] : 1;
    } else {
      shift += std::max(1, j - badchar[txt[shift + j]]);
    }
  }
}

int main() {
  std::string txt = "ABAAABCD";
  std::string pat = "ABC";
  search(txt, pat);
  return 0;
}

/*
 * Boyer Moore - Good suffix heuristic
 */

#include <iostream>

void printArr(int *arr, int n) {
  for (int i = 0; i < n; i++) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;
}

void preprocess_strong_suffix(int *shift, int *bpos, std::string pat, int m) {
  int i = m, j = m + 1;
  bpos[i] = j;

  while (i > 0) {

    while (j <= m && pat[i - 1] != pat[j - 1]) {
      if (shift[j] == 0) {
        std::cout << "shift[" << j << "] = " << (j - i) << std::endl;
        shift[j] = j - i;
      }
      j = bpos[j];
    }
    i--;
    j--;
    bpos[i] = j;
  }
}

void preprocess_case2(int *shift, int *bpos, std::string pat, int m) {
  int i, j;
  j = bpos[0];
  for (i = 0; i <= m; i++) {
    if (shift[i] == 0)
      shift[i] = j;

    if (i == j)
      j = bpos[j];
  }
}

void search(std::string &text, std::string &pat) {
  // s is shift of the pattern with respect to text
  int s = 0, j;
  int m = pat.size();
  int n = text.size();

  int bpos[m + 1], shift[m + 1];

  // initialize all occurrence of shift to 0
  for (int i = 0; i < m + 1; i++)
    shift[i] = 0;

  // do preprocessing
  preprocess_strong_suffix(shift, bpos, pat, m);
  preprocess_case2(shift, bpos, pat, m);

  while (s <= n - m) {

    j = m - 1;

    /* Keep reducing index j of pattern while characters of
         pattern and text are matching at this shift s*/
    while (j >= 0 && pat[j] == text[s + j])
      j--;

    /* If the pattern is present at the current shift, then index j
         will become -1 after the above loop */
    if (j < 0) {
      printf("pattern occurs at shift = %d\n", s);
      s += shift[0];
    } else
      /*pat[i] != pat[s+j] so shift the pattern
        shift[j+1] times  */
      s += shift[j + 1];
  }
}

int main() {
  std::string text = "ABAAAABAACD";
  // std::string pat = "ABA";
  std::string pat = "ABCBAB";
  search(text, pat);
  return 0;
}

#include <iostream>

int main() {
  std::string txt = "hello how are you doing? are you ok ? are you";
  std::string pat = "you";

  int M = pat.size();
  int N = txt.size();

  for (int i = 0; i <= (N - M); i++) {
    int j = 0;
    for (j = 0; j < M; j++) {
      if (pat[j] != txt[i + j]) {
        break;
      }
    }
    if (j == M) {
      std::cout << "Pattern found at index " << i << std::endl;
    }
  }
  return 0;
}

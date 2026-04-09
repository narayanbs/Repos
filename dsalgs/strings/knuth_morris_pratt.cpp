/*
 * Knuth Morris Pratt algorithm
 *
 * Here we preprocess the pattern in that we search for LPS in subpatterns
 * LPS is longest prefix which is also a suffix
 *
 * Example 1: Pattern "aabaaac"

* At index 0: "a" → No proper prefix/suffix → lps[0] = 0
* At index 1: "aa" → "a" is both proper prefix and suffix → lps[1] = 1
* At index 2: "aab" → No prefix matches suffix → lps[2] = 0
* At index 3: "aaba" → "a" is proper prefix and suffix → lps[3] = 1
* At index 4: "aabaa" → "aa" is proper prefix and suffix → lps[4] = 2
* At index 5: "aabaaa" → "aa" is proper prefix and suffix → lps[5] = 2
* At index 6: "aabaaac" → Mismatch, so reset → lps[6] = 0
* Final lps[]: [0, 1, 0, 1, 2, 2, 0]
*
* With this array we can limit the comparisons as follows
*
* Suppose we are searching for the pattern ababc in abababcababc
*
* The first four characters match and then there is a mismatch b/w c and a
* In naive we would have had to start all over again from the beginning of the pattern,
* in KMP we know that before the mismatch there was a and b that was already matched.
* So we can step back to the previous character that had a and b before it. In
* our case it would be the third character a which follows ab.
* We make use of the lps table to jump back to the previous prefix using the suffix.
*
*
*/
#include <iostream>
#include <vector>

std::vector<int> buildLps(const std::string &pat) {
  int m = pat.size();
  std::vector<int> lps(m, 0);

  int j = 0;
  int i = 1;
  while (i < m) {
    if (pat[i] == pat[j]) {
      j++;
      lps[i] = j;
      i++;
    } else {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }
  return lps;
}

void kmp(std::string &txt, std::string &pat) {
  int n = txt.size();
  int m = pat.size();

  if (m == 0 || n == 0 || m > n)
    return;

  std::vector<int> lps = buildLps(pat);

  int i = 0, j = 0;
  while (i < n) {
    if (txt[i] == pat[j]) {
      i++;
      j++;

      if (j == m) {
        std::cout << "Pattern found at index: " << (i - j) << std::endl;
        j = lps[j - 1];
      }
    } else {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }
}

int main() {
  std::string txt = "ABABDABACDABABCABAB";
  std::string pat = "ABABCABAB";
  std::vector<int> res = buildLps(pat);
  for (size_t i = 0; i < res.size(); i++) {
    std::cout << res[i] << " ";
  }
  std::cout << std::endl;
  kmp(txt, pat);
  return 0;
}

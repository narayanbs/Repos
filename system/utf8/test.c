#include <assert.h>
#include <stdlib.h>
#include <string.h>
// clang-format off
#include "utf8.h"

// clang-format on

int main() {
  assert(utf8_is_continuation(0x80));
  assert(!utf8_is_continuation(0xf0));

  assert(utf8_validate("Hello world, je parle français, héhé !"));
  {
    char s[2] = {0xf0, 0x0};
    assert(!utf8_validate(s));
  }

  assert(utf8_strlen("ça é") == 4);

  {
    char s[2] = {0xf0, 0x0};
    assert(utf8_num_bytes(s) == 0);
  }
  assert(utf8_num_bytes("hello") == 1);
  assert(utf8_num_bytes("éello") == 2);

  {
    char s[4] = {0xe0, 0x80, 0x80, 0x0};
    assert(utf8_num_bytes(s) == 3);
  }

  {
    char s[5] = {0xf0, 0x80, 0x80, 0x80, 0x0};
    assert(utf8_num_bytes(s) == 4);
  }

  {
    char *s = utf8_remove_char("Helro", 2);
    assert(strcmp(s, "Hero") == 0);
    free(s);
  }

  {
    char *s = utf8_add_char("Jrémy", "é", 1);
    assert(strcmp(s, "Jérémy") == 0);
    free(s);
  }

  {
    char *s = utf8_replace("déjeuner", "dîner", "Conrad veut déjeuner !");
    assert(strcmp(s, "Conrad veut dîner !") == 0);
    free(s);
  }

  return 0;
}

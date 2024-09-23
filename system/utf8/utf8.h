#ifndef __utf8_h
#define __utf8_h

#include <stddef.h>
#include <stdint.h>

int32_t utf8_is_continuation(char c);
int32_t utf8_validate(char *s);
size_t utf8_strlen(char *s);
size_t utf8_num_bytes(char *s);

int32_t utf8_is_single_byte(char *c);
int32_t utf8_is_double_byte(char *c);
int32_t utf8_is_triple_byte(char *c);
int32_t utf8_is_quadruple_byte(char *c);

char *utf8_remove_trailing_newline(char *s);
char *utf8_remove_char(char *s, size_t n);
char *utf8_add_char(char *s, char *c, size_t n);
char *utf8_replace(char *needle, char *replace, char *haystack);
char *utf8_replace_all(char *needle, char *replace, char *haystack);
char *utf8_escape_null_bytes(const char *s, size_t num);

#endif

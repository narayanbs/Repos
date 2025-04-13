#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ALIGNMENT (sizeof(void *))

typedef struct Stack {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
} Stack;

typedef struct StackHeader {
  size_t prev_offset;
  uint8_t padding;
} StackHeader;

void stack_init(Stack *s, void *buf, size_t buf_len) {
  s->buf = (unsigned char *)buf;
  s->buf_len = buf_len;
  s->prev_offset = 0;
  s->curr_offset = 0;
}

uintptr_t calc_size_with_header(uintptr_t ptr, size_t alignment, size_t header_size) {
  uintptr_t align_ptr = (ptr + (alignment - 1)) & ~(alignment - 1);
  uintptr_t padding = align_ptr - ptr;
  if (padding < header_size) {
    uintptr_t nested_header = header_size - padding;
    if ((nested_header & (alignment - 1)) != 0) {
      padding += alignment * (1 + (nested_header) / (alignment));
    } else {
      padding += alignment * (nested_header / alignment);
    }
  }
  return padding;
}

void *stack_alloc(Stack *s, size_t size) {
  uintptr_t curr_ptr = (uintptr_t)s->buf + (uintptr_t)s->curr_offset;
  uintptr_t padding = calc_size_with_header(curr_ptr, ALIGNMENT, sizeof(StackHeader));
  uintptr_t offset = padding + s->curr_offset;
  if (offset + size < s->buf_len) {
    void *new_ptr = &s->buf[offset];
    memset(new_ptr, 0, size);
    StackHeader *sh = (StackHeader *)((char *)new_ptr - sizeof(StackHeader));
    sh->padding = padding;
    sh->prev_offset = s->prev_offset;
    s->curr_offset = offset + size;
    return new_ptr;
  }
  return NULL;
}

void *stack_free(Stack *s, void *ptr) {
  StackHeader *sh = (StackHeader *)((char *)ptr - sizeof(StackHeader));
  s->prev_offset = sh->prev_offset;
  s->curr_offset = (uintptr_t)ptr - sh->padding - (uintptr_t)s->buf;
  return &s->buf[s->prev_offset];
}

int main(void) {
  char buf[256];
  Stack s = {0};
  stack_init(&s, buf, sizeof(buf));
  int *i;
  float *f;
  i = (int *)stack_alloc(&s, sizeof(int));
  f = (float *)stack_alloc(&s, sizeof(float));
  *i = 20;
  *f = 54.234;
  printf("%d : %p\n", *i, i);
  printf("%f : %p\n", *f, f);
  stack_free(&s, f);
  printf("%d : %p\n", *i, i);
  return 0;
}

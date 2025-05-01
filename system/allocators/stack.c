#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ALIGNMENT (sizeof(void *))

typedef struct Stack {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
} Stack;

typedef struct StackHeader {
  size_t padding;
  size_t prev_offset;
} StackHeader;

void stack_init(Stack *s, void *buf, size_t size) {
  s->buf = (unsigned char *)buf;
  s->buf_len = size;
  s->prev_offset = 0;
  s->curr_offset = 0;
}

uintptr_t calc_size_with_header(uintptr_t curr_addr, uintptr_t align, uintptr_t header_size) {
  uintptr_t aligned_addr = (curr_addr + align - 1) & ~(align - 1);
  uintptr_t padding = aligned_addr - curr_addr;
  if (padding < header_size) {
    uintptr_t nested_header = header_size - padding;
    if ((nested_header & (align - 1)) != 0) {
      padding += align * (1 + nested_header / align);
    } else {
      padding += align * (nested_header / align);
    }
  }
  return padding;
}

void *stack_alloc(Stack *s, size_t size) {
  uintptr_t curr_addr = (uintptr_t)s->buf + (uintptr_t)s->curr_offset;
  size_t padding = calc_size_with_header(curr_addr, ALIGNMENT, sizeof(StackHeader));
  size_t offset = s->curr_offset + padding;
  if (offset + size > s->buf_len) {
    return NULL;
  }
  uintptr_t aligned_addr = (uintptr_t)s->buf + offset;
  StackHeader *sh = (StackHeader *)(aligned_addr - sizeof(StackHeader));
  sh->prev_offset = s->prev_offset;
  sh->padding = padding;
  s->prev_offset = s->curr_offset;
  s->curr_offset = offset + size;
  void *ptr = (void *)aligned_addr;
  memset(ptr, 0, size);
  return ptr;
}

void stack_free(Stack *s, void *ptr) {
  if (ptr == NULL) return;
  uintptr_t start = (uintptr_t)s->buf;
  uintptr_t end = start + (uintptr_t)s->buf_len;
  uintptr_t curr_addr = (uintptr_t)ptr;
  if (!(start <= curr_addr && curr_addr < end)) {
    assert(0 && "Out of bounds memory address passed to stack allocator (free)");
    return;
  }
  if (curr_addr >= (start + (uintptr_t)s->curr_offset)) {
    return;
  }
  StackHeader *sh = (StackHeader *)(curr_addr - sizeof(StackHeader));
  uintptr_t prev_offset = curr_addr - sh->padding - start;
  if (prev_offset != s->prev_offset) {
    assert(0 && "Out of order stack allocator (free)");
    return;
  }
  s->curr_offset = s->prev_offset;
  s->prev_offset = sh->prev_offset;
}

int main(void) {
  char buf[256];
  Stack s = {0};
  stack_init(&s, buf, sizeof(buf));
  int *i;
  i = stack_alloc(&s, sizeof(int));
}

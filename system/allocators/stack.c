#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Stack Stack;

struct Stack {
  unsigned char *buf;
  size_t buf_len;
  size_t offset;
};

typedef struct Stack_Allocation_Header Stack_Allocation_Header;

struct Stack_Allocation_Header {
  uint8_t padding;
};

bool is_power_of_two(size_t x) {
  return (x & (x - 1)) == 0;
}

void stack_init(Stack *s, void *backing_buffer, size_t backing_buffer_length) {
  s->buf = (unsigned char *)backing_buffer;
  s->buf_len = backing_buffer_length;
  s->offset = 0;
}

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, uintptr_t header_size) {
  uintptr_t p, a, padding, modulo, needed_space;

  assert(is_power_of_two(alignment));

  p = ptr;
  a = alignment;

  modulo = p & (a - 1);

  padding = 0;
  needed_space = 0;

  if (modulo != 0) {
    padding = a - modulo;
  }
  needed_space = (uintptr_t)header_size;

  if (padding < needed_space) {
    needed_space -= padding;
    if ((needed_space & (a - 1)) != 0) {
      padding += a * (1 + (needed_space / a));
    } else {
      padding += a * (needed_space / a);
    }
  }
  return (size_t)padding;
}

void *stack_align_alloc(Stack *s, size_t size, size_t alignment) {
  uintptr_t curr_addr, next_addr;
  size_t padding;
  Stack_Allocation_Header *header;

  assert(is_power_of_two(alignment));

  if (alignment > 128) {
    alignment = 128;
  }

  curr_addr = (uintptr_t)s->buf + (uintptr_t)s->offset;
  padding = calc_padding_with_header(curr_addr, (uintptr_t)alignment, sizeof(Stack_Allocation_Header));
  if (s->offset + padding + size > s->buf_len) {
    return NULL;
  }
  s->offset += padding;

  next_addr = curr_addr + (uintptr_t)padding;
  header = (Stack_Allocation_Header *)(next_addr - sizeof(Stack_Allocation_Header));
  header->padding = (uint8_t)padding;

  s->offset += size;
  return memset((void *)next_addr, 0, size);
}

void *stack_alloc(Stack *s, size_t size) {
  return stack_align_alloc(s, size, DEFAULT_ALIGNMENT);
}

void stack_free(Stack *s, void *ptr) {
  if (ptr != NULL) {
    uintptr_t start, end, curr_addr;
    Stack_Allocation_Header *header;
    size_t prev_offset;

    start = (uintptr_t)s->buf;
    end = start + (uintptr_t)s->buf_len;
    curr_addr = (uintptr_t)ptr;

    if (!(start <= curr_addr && curr_addr < end)) {
      assert(0 && "Out of bounds memory address passed to stack allocator (free)");
      return;
    }

    if (curr_addr >= start + (uintptr_t)s->offset) {
      return;
    }

    header = (Stack_Allocation_Header *)(curr_addr - sizeof(Stack_Allocation_Header));
    prev_offset = (size_t)(curr_addr - (uintptr_t)header->padding - start);
    s->offset = prev_offset;
  }
}

void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, size_t alignment) {
  if (ptr == NULL) {
    return stack_align_alloc(s, new_size, alignment);
  } else if (new_size == 0) {
    stack_free(s, ptr);
    return NULL;
  } else {
    uintptr_t start, end, curr_addr;
    size_t min_size = old_size < new_size ? old_size : new_size;
    void *new_ptr;

    start = (uintptr_t)s->buf;
    end = start + (uintptr_t)s->buf_len;
    curr_addr = (uintptr_t)ptr;
    if (!(start <= curr_addr && curr_addr < end)) {
      assert(0 && "Out of bounds memory address passed to stack allocator (resize)");
      return NULL;
    }

    if (curr_addr >= start + (uintptr_t)s->offset) {
      // Treat as a double free
      return NULL;
    }

    if (old_size == new_size) {
      return ptr;
    }

    new_ptr = stack_align_alloc(s, new_size, alignment);
    memmove(new_ptr, ptr, min_size);
    return new_ptr;
  }
}

void *stack_resize(Stack *s, void *ptr, size_t old_size, size_t new_size) {
  return stack_resize_align(s, ptr, old_size, new_size, DEFAULT_ALIGNMENT);
}

void stack_free_all(Stack *s) {
  s->offset = 0;
}

int main(void) {
  unsigned char backing_buffer[256];
  Stack s = {0};
  stack_init(&s, backing_buffer, 256);

  int *x;
  float *f;
  char *str;

  // Reset all stack offsets for each loop
  stack_free_all(&s);

  x = (int *)stack_alloc(&s, sizeof(int));
  *x = 123;

  f = (float *)stack_alloc(&s, sizeof(float));
  *f = 987;

  str = stack_alloc(&s, 10);
  memmove(str, "Hellope", 7);
  printf("%p: %s\n", str, str);

  str = stack_resize(&s, str, 10, 16);
  memmove(str + 7, " world!", 7);
  printf("%p: %s\n", str, str);
  stack_free(&s, str);

  printf("%p: %f\n", f, *f);
  stack_free(&s, f);

  printf("%p: %d\n", x, *x);
  stack_free(&s, x);

  stack_free_all(&s);

  return 0;
}

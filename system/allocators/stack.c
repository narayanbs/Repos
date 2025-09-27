#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ALIGNMENT (sizeof(void *))

typedef struct Stack Stack;

struct Stack {
  unsigned char *buf;
  size_t buflen;
  size_t prev_frame_offset;
  size_t curr_frame_offset;
};

typedef struct StackHeader StackHeader;

struct StackHeader {
  size_t padding;
  size_t prev_offset;
};

void stack_init(Stack *s, void *buf, size_t length) {
  s->buf = buf;
  s->buflen = length;
  s->prev_frame_offset = 0;
  s->curr_frame_offset = 0;
}

uintptr_t calc_padding(uintptr_t ptr, size_t align, size_t header_size) {
  uintptr_t aligned_ptr = (ptr + align - 1) & ~(align - 1);
  uintptr_t padding = aligned_ptr - ptr;
  if (padding < header_size) {
    uintptr_t nested_header = header_size - padding;
    padding += align * ((nested_header + align - 1) / align);
  }
  return padding;
}

void *stack_alloc(Stack *s, size_t size) {
  uintptr_t curr_ptr = (uintptr_t)s->buf + s->curr_frame_offset;
  size_t padding = calc_padding(curr_ptr, ALIGNMENT, sizeof(StackHeader));
  uintptr_t aligned_ptr = curr_ptr + padding;
  size_t offset = aligned_ptr - (uintptr_t)s->buf;
  if (offset + size <= s->buflen) {
    void *ptr = (void *)curr_ptr + padding;
    StackHeader *sh = (StackHeader *)((char *)ptr - sizeof(StackHeader));
    sh->padding = padding;
    sh->prev_offset = s->prev_frame_offset;
    s->prev_frame_offset = s->curr_frame_offset;
    s->curr_frame_offset = offset + size;
    return ptr;
  }
  return NULL;
}

void stack_free(Stack *s, void *ptr) {
  StackHeader *sh = (StackHeader *)((char *)ptr - sizeof(StackHeader));
  void *prev_ptr = (char *)ptr - sh->padding;
  uintptr_t prev_offset = (uintptr_t)prev_ptr - (uintptr_t)s->buf;
  if (prev_offset != s->prev_frame_offset) {
    assert(0 && "Invalid stack frame to unload");
    return;
  }
  s->curr_frame_offset = s->prev_frame_offset;
  s->prev_frame_offset = sh->prev_offset;
}

int main(void) {
  char buf[256];
  Stack s = {0};
  stack_init(&s, buf, sizeof(buf));

  // ✅ Test 1: Allocate int
  int *i = stack_alloc(&s, sizeof(int));
  assert(i != NULL);
  *i = 42;
  printf("Test 1: int = %d\n", *i);

  // ✅ Test 2: Allocate double
  double *d = stack_alloc(&s, sizeof(double));
  assert(d != NULL);
  *d = 3.14159;
  printf("Test 2: double = %f\n", *d);

  // ✅ Test 3: Allocate a struct
  typedef struct {
    int x;
    float y;
    char name[16];
  } TestStruct;

  TestStruct *ts = stack_alloc(&s, sizeof(TestStruct));
  assert(ts != NULL);
  ts->x = 123;
  ts->y = 4.56f;
  strcpy(ts->name, "StackAlloc");
  printf("Test 3: struct = { %d, %.2f, %s }\n", ts->x, ts->y, ts->name);

  // ✅ Test 4: Allocate array
  int *arr = stack_alloc(&s, sizeof(int) * 10);
  assert(arr != NULL);
  for (int j = 0; j < 10; j++) arr[j] = j * j;
  printf("Test 4: array = ");
  for (int j = 0; j < 10; j++) printf("%d ", arr[j]);
  printf("\n");

  // ✅ Test 5: Free array
  stack_free(&s, arr);

  // ✅ Test 6: Free struct
  stack_free(&s, ts);

  // ✅ Test 7: Free double
  stack_free(&s, d);

  // ✅ Test 8: Free int
  stack_free(&s, i);

  // ✅ Test 9: Reuse after freeing
  char *text = stack_alloc(&s, 20);
  assert(text != NULL);
  strcpy(text, "Reused buffer!");
  printf("Test 9: text = %s\n", text);

  // ✅ Test 10: Allocate nearly full buffer
  void *big_block = stack_alloc(&s, 240);  // Too big for remaining space
  assert(big_block == NULL);               // Should fail

  printf("All tests passed.\n");

  return 0;
}

/*
 * stack allocator is similar to arena, in that we allocate a frame of the required size
 * and adjusting the appropriate offsets.
 * We can also unwind the stack and remove the current frame and make the previous frame active.
 * so we need to keep the offsets of the previous frame in memory when we create a new frame. For
 * this we use the header. The header stores the previous frame offset and the padding from the
 * current frame to the previous frame.
 * So when we free a frame, we just need to adjust the offsets to unwind the current one and make
 * the previous one active.
 *
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_ALIGNMENT (sizeof(void *))

typedef struct Stack Stack;

struct Stack {
  unsigned char *buf;
  size_t length;
  size_t frame_start_offset;
  size_t frame_end_offset;
};

typedef struct StackHeader StackHeader;

struct StackHeader {
  size_t prev_frame_start_offset;
  size_t prev_frame_padding;
};

void stack_init(Stack *s, void *buf, size_t buf_length) {
  s->buf = buf;
  s->length = buf_length;
  s->frame_start_offset = 0;
  s->frame_end_offset = 0;
}

uintptr_t align_alloc(uintptr_t ptr, uintptr_t align) {
  return (ptr + align - 1) & ~(align - 1);
}

void *stack_alloc(Stack *s, size_t size) {
  // the stack allocation size includes the header size
  // check if the size is within limits
  size_t newframesize = sizeof(StackHeader) + size;
  if (s->frame_end_offset + newframesize > s->length) {
    return NULL;
  }
  // Address of the end of the current frame
  uintptr_t curr_ptr = (uintptr_t)s->buf + (uintptr_t)s->frame_end_offset;
  // Aligned address of the end of the current frame
  uintptr_t aligned_ptr = align_alloc(curr_ptr, DEFAULT_ALIGNMENT);

  // The first part includes the StackHeader
  StackHeader *sh = (void *)aligned_ptr;

  // Store the start offset of existing frame in StackHeader
  sh->prev_frame_start_offset = s->frame_start_offset;
  // Store the padding from the new frame to existing frame in StackHeader
  sh->prev_frame_padding = (uintptr_t)aligned_ptr - s->frame_end_offset - (uintptr_t)s->buf;

  // Update the offsets of the new frame
  s->frame_start_offset = (uintptr_t)aligned_ptr - (uintptr_t)s->buf;
  s->frame_end_offset = s->frame_start_offset + newframesize;
  printf("allocating: start offset %zu and end offset %zu\n", s->frame_start_offset,
         s->frame_end_offset);

  // Now return the pointer to the payload (sans the StackHeader)
  return (void *)aligned_ptr + sizeof(StackHeader);
}

void *stack_free(Stack *s, void *ptr) {
  // Obtain the StackHeader to get the start offset of the previous frame
  // and the paddding between the current and previous frame
  StackHeader *sh = (void *)ptr - sizeof(StackHeader);

  // Update the offsets of the stack to the previous frame
  s->frame_end_offset = (uintptr_t)sh - sh->prev_frame_padding - (uintptr_t)s->buf;
  s->frame_start_offset = sh->prev_frame_start_offset;
  printf("freeing : start offset %zu and end offset %zu\n", s->frame_start_offset,
         s->frame_end_offset);

  // Return the pointer to the payload
  return (void *)&s->buf[s->frame_start_offset] + sizeof(StackHeader);
}

int main(void) {
  char buf[512];
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
  void *big_block = stack_alloc(&s, 500);  // Too big for remaining space
  assert(big_block == NULL);               // Should fail

  printf("All tests passed.\n");

  return 0;
}

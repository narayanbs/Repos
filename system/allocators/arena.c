/*
 * Arena allocator
 *
 * buffer of memory aligned to the given boundary
 * start_frame_offset and end_frame_offset hold the start and end distances of the current frame
 * from the beginning of the arena block
 *
 * when a new request for allocation is made, we make sure that a new frame that is properly aligned
 * is created. We adjust the offsets to the newly created frame distances.
 *
 * When a request for resize is made, we check for the following conditions
 * 1. If the pointer does not point to an existing frame we create a new frame with the given new
 * size
 * 2. If the pointer points to an existing frame
 *  a) then if it is the current frame, we just need to adjust the offset distances to the new size
 *  b) else we create a new frame with the new size and  copy the contents of the frame we need to
 * resize
 *
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool is_power_of_two(uintptr_t x) {
  return (x & (x - 1)) == 0;
}

/*
 * Obtain the next aligned address
 */
uintptr_t align_forward(uintptr_t ptr, size_t align) {
  uintptr_t p, a, modulo;
  assert(is_power_of_two(align));
  p = ptr;
  a = (uintptr_t)align;
  modulo = p & (a - 1);
  if (modulo != 0) {
    p += a - modulo;
  }
  return p;
  // The above can be replaced by the following bitwise operation
  // return (ptr + align - 1) & ~(align - 1);
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Arena Arena;

struct Arena {
  unsigned char *buf;
  size_t buf_len;
  size_t start_frame_offset;
  size_t end_frame_offset;
};

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_length) {
  a->buf = (unsigned char *)backing_buffer;
  a->buf_len = backing_buffer_length;
  a->start_frame_offset = 0;
  a->end_frame_offset = 0;
}

void *arena_alloc_align(Arena *a, size_t size, size_t align) {
  uintptr_t curr_ptr = (uintptr_t)a->buf + (uintptr_t)a->end_frame_offset;
  uintptr_t aligned_ptr = align_forward(curr_ptr, align);
  uintptr_t offset = aligned_ptr - (uintptr_t)a->buf;

  if (offset + size <= a->buf_len) {
    void *ptr = &a->buf[offset];
    a->start_frame_offset = offset;
    a->end_frame_offset = offset + size;

    memset(ptr, 0, size);
    return ptr;
  }
  return NULL;
}

void *arena_alloc(Arena *a, size_t size) {
  return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void arena_free(Arena *a, void *ptr) {
  // Do nothing
}

void *arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size,
                         size_t align) {
  unsigned char *old_mem = (unsigned char *)old_memory;

  assert(is_power_of_two(align));

  // return a new arena frame in case the pointer is NULL or size is zero
  if (old_mem == NULL || old_size == 0) {
    return arena_alloc_align(a, new_size, align);
  }
  // else check if the pointer is within limits of the arena
  else if (a->buf <= old_mem && old_mem < a->buf + a->buf_len) {
    // if the pointer points to the latest frame then
    // resize it
    if (a->buf + a->start_frame_offset == old_mem) {
      if (new_size > old_size) {
        memset(&a->buf[a->end_frame_offset], 0, new_size - old_size);
      }
      a->end_frame_offset = a->start_frame_offset + new_size;
      return old_memory;
    } else {
      // else if pointer points to another frame then
      // create a new frame and copy the contents to it.
      void *new_memory = arena_alloc_align(a, new_size, align);
      size_t copy_size = old_size < new_size ? old_size : new_size;
      memmove(new_memory, old_memory, copy_size);
      return new_memory;
    }
  } else {
    // Cannot resize the arena
    assert(0 && "Memory is out of bounds of the buffer in this arena");
    return NULL;
  }
}

void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size) {
  return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

void arena_free_all(Arena *a) {
  a->start_frame_offset = 0;
  a->end_frame_offset = 0;
}

// Extra Features
typedef struct Temp_Arena_Memory Temp_Arena_Memory;

struct Temp_Arena_Memory {
  Arena *arena;
  size_t prev_offset;
  size_t curr_offset;
};

Temp_Arena_Memory temp_arena_memory_begin(Arena *a) {
  Temp_Arena_Memory temp;
  temp.arena = a;
  temp.prev_offset = a->start_frame_offset;
  temp.curr_offset = a->end_frame_offset;
  return temp;
}

void temp_arena_memory_end(Temp_Arena_Memory temp) {
  temp.arena->start_frame_offset = temp.prev_offset;
  temp.arena->end_frame_offset = temp.curr_offset;
}

/*
 * main function
 */
int main(int argc, char **argv) {
  int i;

  unsigned char backing_buffer[256];
  Arena a = {0};
  arena_init(&a, backing_buffer, 256);

  for (i = 0; i < 1; i++) {
    int *x;
    float *f;
    char *str;

    // Reset all arena offsets for each loop
    arena_free_all(&a);

    x = (int *)arena_alloc(&a, sizeof(int));
    f = (float *)arena_alloc(&a, sizeof(float));
    str = arena_alloc(&a, 10);

    *x = 123;
    *f = 987;
    memmove(str, "Hellope", 7);

    printf("%p: %d\n", x, *x);
    printf("%p: %f\n", f, *f);
    printf("%p: %s\n", str, str);

    str = arena_resize(&a, str, 10, 16);
    memmove(str + 7, " world!", 7);
    printf("%p: %s\n", str, str);
  }

  arena_free_all(&a);

  return 0;
}

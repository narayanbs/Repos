#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_POOL_SIZE (1024 * 1024) // 1MB memory pool
#define MAX_LEVELS 21 // log2(1MB) = 20, safe upper limit for free list

// Structure to represent a block of memory
typedef struct Block {
  size_t size;        // Size of the block (power of 2)
  int free;           // Whether the block is free (1) or used (0)
  struct Block *next; // Pointer to the next block in the free list
} Block;

// Memory pool and free list
static char memory_pool[MEMORY_POOL_SIZE];
static Block *free_list[MAX_LEVELS];

// Utility: integer log base 2
int log2_size(size_t size) {
  int log = 0;
  while (size >>= 1)
    ++log;
  return log;
}

// Round up to next power of 2
size_t next_power_of_two(size_t size) {
  size_t power = 1;
  while (power < size)
    power <<= 1;
  return power;
}

// Initialize buddy allocator
void buddy_allocator_init() {
  memset(free_list, 0, sizeof(free_list));

  Block *initial_block = (Block *)memory_pool;
  initial_block->size = MEMORY_POOL_SIZE;
  initial_block->free = 1;
  initial_block->next = NULL;

  int index = log2_size(MEMORY_POOL_SIZE);
  free_list[index] = initial_block;
}

// Add a block to the free list
void add_to_free_list(Block *block) {
  int index = log2_size(block->size);
  block->free = 1;
  block->next = free_list[index];
  free_list[index] = block;
}

// Remove and return the first block from a free list
Block *remove_from_free_list(int index) {
  if (free_list[index] == NULL)
    return NULL;
  Block *block = free_list[index];
  free_list[index] = block->next;
  block->next = NULL;
  return block;
}

// Split a block into two halves
void split_block(Block *block, int index) {
  size_t new_size = block->size / 2;

  Block *buddy = (Block *)((char *)block + new_size);
  buddy->size = new_size;
  buddy->free = 1;
  buddy->next = NULL;

  block->size = new_size;
  block->free = 1;

  // Add both halves to the smaller size free list
  add_to_free_list(buddy);
}

// Find a suitable block for allocation
Block *find_block(size_t alloc_size) {
  int index = log2_size(alloc_size);

  for (int i = index; i < MAX_LEVELS; i++) {
    if (free_list[i] != NULL) {
      Block *block = remove_from_free_list(i);

      // Split until block size matches required size
      while (block->size > alloc_size) {
        split_block(block, log2_size(block->size));
      }

      block->free = 0;
      return block;
    }
  }

  return NULL; // No suitable block found
}

// Calculate buddy address using XOR trick
Block *get_buddy(Block *block) {
  size_t offset = (char *)block - memory_pool;
  size_t buddy_offset = offset ^ block->size;
  if (buddy_offset >= MEMORY_POOL_SIZE)
    return NULL;
  return (Block *)(memory_pool + buddy_offset);
}

// Remove a block from its free list (safe)
void remove_from_free_list_safe(Block *block) {
  int index = log2_size(block->size);
  Block **curr = &free_list[index];

  while (*curr) {
    if (*curr == block) {
      *curr = block->next;
      block->next = NULL;
      return;
    }
    curr = &(*curr)->next;
  }
}

// Recursively coalesce free buddies
void coalesce(Block *block) {
  while (block->size < MEMORY_POOL_SIZE) {
    Block *buddy = get_buddy(block);
    if (!buddy || !buddy->free || buddy->size != block->size) {
      break;
    }

    // Remove both from current free list
    remove_from_free_list_safe(buddy);
    remove_from_free_list_safe(block);

    // Merge
    if (block > buddy) {
      Block *temp = block;
      block = buddy;
      buddy = temp;
    }

    block->size *= 2;
    block->free = 1;
    block->next = NULL;
  }

  add_to_free_list(block);
}

// Allocate memory
void *buddy_malloc(size_t size) {
  if (size == 0 || size > MEMORY_POOL_SIZE - sizeof(Block))
    return NULL;

  size_t total_size = size + sizeof(Block);
  size_t alloc_size = next_power_of_two(total_size);

  Block *block = find_block(alloc_size);
  if (!block)
    return NULL;

  return (char *)block + sizeof(Block);
}

// Free memory
void buddy_free(void *ptr) {
  if (!ptr)
    return;

  Block *block = (Block *)((char *)ptr - sizeof(Block));
  block->free = 1;

  coalesce(block);
}

// Debug print
void print_free_list() {
  printf("Free list status:\n");
  for (int i = 0; i < MAX_LEVELS; i++) {
    if (free_list[i]) {
      printf("  Size %zu: ", (size_t)1 << i);
      Block *curr = free_list[i];
      while (curr) {
        printf("[%p size=%zu] -> ", curr, curr->size);
        curr = curr->next;
      }
      printf("NULL\n");
    }
  }
}

// Example usage
int main() {
  buddy_allocator_init();

  printf("Before allocation:\n");
  print_free_list();

  void *a = buddy_malloc(100);
  void *b = buddy_malloc(200);
  void *c = buddy_malloc(50);

  printf("\nAfter allocation:\n");
  print_free_list();

  buddy_free(a);
  buddy_free(b);
  buddy_free(c);

  printf("\nAfter free:\n");
  print_free_list();

  return 0;
}

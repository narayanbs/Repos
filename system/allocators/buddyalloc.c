#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to represent a block of memory.
typedef struct Block {
  size_t size;         // Size of the block (power of 2)
  int free;            // Whether the block is free (1) or used (0)
  struct Block* next;  // Pointer to the next block in the free list
} Block;

// Global variables for the memory pool
#define MEMORY_POOL_SIZE (1024 * 1024)      // 1MB memory pool
static char memory_pool[MEMORY_POOL_SIZE];  // Memory pool for allocation
static Block* free_list[32];                // Array of free lists (one for each power of 2)

// Utility function to calculate the log base 2 of a number
int log2_size(size_t size) {
  return (int)(log(size) / log(2));
}

// Initialize the memory pool and free list
void buddy_allocator_init() {
  // Initialize all free lists to NULL
  memset(free_list, 0, sizeof(free_list));

  // Create a single large block (initially the entire pool)
  Block* initial_block = (Block*)memory_pool;
  initial_block->size = MEMORY_POOL_SIZE;
  initial_block->free = 1;
  initial_block->next = NULL;

  // Add the initial block to the appropriate free list
  int index = log2_size(MEMORY_POOL_SIZE);
  free_list[index] = initial_block;
}

// Find a block of a given size
Block* find_block(size_t size) {
  int index = log2_size(size);

  // Find the smallest block that is big enough
  for (int i = index; i < 32; i++) {
    if (free_list[i] != NULL) {
      Block* block = free_list[i];
      free_list[i] = block->next;  // Remove from the free list

      // Split the block if it's larger than required
      while (block->size > size) {
        Block* buddy = (Block*)((char*)block + block->size / 2);
        buddy->size = block->size / 2;
        buddy->free = 1;
        buddy->next = free_list[log2_size(buddy->size)];
        free_list[log2_size(buddy->size)] = buddy;

        block->size /= 2;
      }

      block->free = 0;
      return block;
    }
  }

  // No suitable block found
  return NULL;
}

// Allocate memory from the buddy allocator
void* buddy_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  // Round up the size to the next power of 2
  size_t alloc_size = 1;
  while (alloc_size < size) {
    alloc_size *= 2;
  }

  Block* block = find_block(alloc_size);
  if (block == NULL) {
    return NULL;  // No suitable block available
  }
  return (char*)block + sizeof(Block);
}

// Coalesce adjacent free blocks
void coalesce(Block* block) {
  size_t size = block->size;
  Block* buddy = (Block*)((char*)block + block->size);

  // if (buddy->size == 0) return;

  // Check if the buddy is free and adjacent
  if (buddy->free && (buddy->size == size)) {
    // Remove buddy from the free list
    Block** prev = &free_list[log2_size(buddy->size)];
    while (*prev != buddy) {
      prev = &(*prev)->next;
    }
    *prev = buddy->next;

    // Merge with buddy (half the size)
    if (block > buddy) {
      Block* temp = block;
      block = buddy;
      buddy = temp;
    }

    block->size *= 2;
    block->next = NULL;

    // Add the merged block back to the free list
    free_list[log2_size(block->size)] = block;
  }
}

// Free a previously allocated block
void buddy_free(void* ptr) {
  if (ptr == NULL) {
    return;
  }

  // Get the block header from the given pointer
  Block* block = (Block*)((char*)ptr - sizeof(Block));
  block->free = 1;

  // Coalesce the block with its buddy if possible
  coalesce(block);
}

// Print memory pool information (for debugging)
void print_free_list() {
  for (int i = 0; i < 32; i++) {
    if (free_list[i] != NULL) {
      printf("Free list[%d]: ", 1 << i);
      Block* current = free_list[i];
      while (current != NULL) {
        printf("Block at %p (size: %zu) -> ", current, current->size);
        current = current->next;
      }
      printf("NULL\n");
    }
  }
}

// Example usage of the buddy allocator
int main() {
  buddy_allocator_init();

  printf("Before allocation:\n");
  print_free_list();

  void* block1 = buddy_malloc(100);
  void* block2 = buddy_malloc(200);
  void* block3 = buddy_malloc(50);

  printf("\nAfter allocations:\n");
  print_free_list();

  buddy_free(block1);
  buddy_free(block2);
  buddy_free(block3);

  printf("\nAfter freeing blocks:\n");
  print_free_list();

  return 0;
}

/*
 * This freelist allocator implements both find first and find best policy.
 * The default is find best, To enable find first pass -DFIRST_PLACEMENT during
 * compilation step clang freelist.c -Wall -DFIRST_PLACEMENT
 * -fsanitize=alignment -fsanitize=undefined -o freelist
 *
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Minimum alignment supported
#define MIN_ALIGNMENT 8

// Enum representing allocation strategies
typedef enum {
  Placement_Policy_Find_First,  // First fit
  Placement_Policy_Find_Best    // Best fit
} Placement_Policy;

// Free list node structure representing a free block in the allocator
typedef struct FreeList_Node {
  struct FreeList_Node *next;
  size_t block_size;
} FreeList_Node;

// Header placed before every allocated block to track metadata
typedef struct {
  size_t block_size;
  size_t padding;
} FreeList_Header;

// Allocator structure
typedef struct {
  void *data;               // Pointer to memory pool
  size_t size;              // Total size of memory pool
  size_t used;              // Used bytes
  FreeList_Node *head;      // Head of the free list
  Placement_Policy policy;  // Allocation strategy
} FreeList;

// Resets the entire memory to a single free block
void freelist_free_all(FreeList *fl) {
  fl->used = 0;

  FreeList_Node *firstnode = (FreeList_Node *)fl->data;
  firstnode->block_size = fl->size;
  firstnode->next = NULL;

  fl->head = firstnode;
}

// Initializes the free list allocator with a given buffer
void free_list_init(FreeList *fl, void *data, size_t size) {
  fl->data = data;
  fl->size = size;
  fl->policy = Placement_Policy_Find_First;  // Default strategy
  freelist_free_all(fl);
}

// Inserts a node into the free list after prev_node
void freelist_node_insert(FreeList_Node **phead, FreeList_Node *prev_node,
                          FreeList_Node *new_node) {
  if (prev_node == NULL) {
    new_node->next = *phead;
    *phead = new_node;
  } else {
    new_node->next = prev_node->next;
    prev_node->next = new_node;
  }
}

// Removes a node from the free list
void freelist_node_remove(FreeList_Node **phead, FreeList_Node *prev_node,
                          FreeList_Node *del_node) {
  if (prev_node == NULL) {
    *phead = del_node->next;
  } else {
    prev_node->next = del_node->next;
  }
}

void print_freelist(FreeList *fl) {
  FreeList_Node *node = fl->head;
  printf("\n");
  while (node != NULL) {
    printf("FreeList_Node %p --> ", (void *)node);
    node = node->next;
  }
  printf("\n");
}

// Checks if a value is a power of two
bool is_power_of_two(uintptr_t x) {
  return (x & (x - 1)) == 0;
}

// Calculates the required padding for alignment and header
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
  assert(is_power_of_two(alignment));

  uintptr_t modulo = ptr & (alignment - 1);
  size_t padding = (modulo != 0) ? alignment - modulo : 0;

  if (padding < header_size) {
    size_t needed_space = header_size - padding;
    padding += alignment * ((needed_space + alignment - 1) / alignment);
  }

  return padding;
}

// Finds the first block that fits (first-fit strategy)
FreeList_Node *freelist_find_first(FreeList *fl, size_t size, size_t alignment, size_t *padding_,
                                   FreeList_Node **prev_node_) {
  FreeList_Node *node = fl->head;
  FreeList_Node *prev_node = NULL;

  while (node != NULL) {
    size_t padding = calc_padding_with_header((uintptr_t)node, alignment, sizeof(FreeList_Header));
    size_t required_space = size + padding;

    if (node->block_size >= required_space) {
      if (padding_) *padding_ = padding;
      if (prev_node_) *prev_node_ = prev_node;
      return node;
    }

    prev_node = node;
    node = node->next;
  }

  return NULL;
}

// Finds the best fitting block (best-fit strategy)
FreeList_Node *freelist_find_best(FreeList *fl, size_t size, size_t alignment, size_t *padding_,
                                  FreeList_Node **prev_node_) {
  size_t smallest_diff = ~(size_t)0;
  FreeList_Node *node = fl->head;
  FreeList_Node *prev_node = NULL;
  FreeList_Node *best_node = NULL;
  FreeList_Node *best_prev = NULL;
  size_t best_padding = 0;

  while (node != NULL) {
    size_t padding = calc_padding_with_header((uintptr_t)node, alignment, sizeof(FreeList_Header));
    size_t required_space = size + padding;

    if (node->block_size >= required_space) {
      size_t diff = node->block_size - required_space;
      if (diff < smallest_diff) {
        best_node = node;
        best_prev = prev_node;
        best_padding = padding;
        smallest_diff = diff;
      }
    }

    prev_node = node;
    node = node->next;
  }

  if (padding_) *padding_ = best_padding;
  if (prev_node_) *prev_node_ = best_prev;
  return best_node;
}

// Allocates memory from the free list
void *freelist_alloc(FreeList *fl, size_t size, size_t alignment) {
  // Enforce minimum allocation and alignment
  if (size < sizeof(FreeList_Node)) size = sizeof(FreeList_Node);
  if (alignment < MIN_ALIGNMENT) alignment = MIN_ALIGNMENT;

  size_t padding = 0;
  FreeList_Node *prev_node = NULL;

  // Choose placement strategy
  FreeList_Node *node = (fl->policy == Placement_Policy_Find_Best)
                            ? freelist_find_best(fl, size, alignment, &padding, &prev_node)
                            : freelist_find_first(fl, size, alignment, &padding, &prev_node);

  if (node == NULL) {
    assert(0 && "Free list has no free memory");
    return NULL;
  }

  size_t alignment_padding = padding - sizeof(FreeList_Header);
  size_t required_space = size + padding;
  size_t remaining = node->block_size - required_space;

  // Debug print allocation request and chosen block info
  printf("[ALLOC] Request size=%zu, alignment=%zu\n", size, alignment);
  printf("[ALLOC] Using free block at %p, size=%zu\n", (void *)node, node->block_size);

  // space gained through alignment
  size_t extra_space = 0;

  if (remaining >= sizeof(struct FreeList_Node)) {
    // make sure it's aligned
    void *ptr = (char *)node + required_space;
    uintptr_t curr_ptr = (uintptr_t)ptr;
    uintptr_t aligned_ptr = (curr_ptr + alignment - 1) & ~(alignment - 1);
    extra_space = aligned_ptr - curr_ptr;

    if ((remaining - extra_space) >= sizeof(struct FreeList_Node)) {
      // Instead of removing node then inserting, just adjust current node and
      // insert new_node in its place
      FreeList_Node *new_node = (FreeList_Node *)((void *)aligned_ptr);
      new_node->block_size = remaining - extra_space;
      freelist_node_insert(&fl->head, node, new_node);

      printf("Remaining block %p of size %zu\n", (void *)new_node, remaining - extra_space);
    }
  }

  required_space += extra_space;
  // Update the current node size to required_space, effectively splitting it
  node->block_size = required_space;
  // Remove the node from free list as it is fully allocated
  freelist_node_remove(&fl->head, prev_node, node);

  // Setup allocation header and return aligned pointer
  FreeList_Header *header_ptr = (FreeList_Header *)((char *)node + alignment_padding);
  header_ptr->block_size = required_space;
  header_ptr->padding = alignment_padding;

  fl->used += required_space;

  return (void *)((char *)header_ptr + sizeof(FreeList_Header));
}

// Merges adjacent free blocks
void freelist_coalescence(FreeList *fl, FreeList_Node *prev_node, FreeList_Node *free_node) {
  // Coalesce with next
  if (free_node->next != NULL &&
      (void *)((char *)free_node + free_node->block_size) == (void *)free_node->next) {
    printf("Coalescing free block [%p] with next block [%p] ...\n", (void *)free_node,
           (void *)free_node->next);
    free_node->block_size += free_node->next->block_size;
    freelist_node_remove(&fl->head, free_node, free_node->next);
  }

  // Coalesce with previous
  if (prev_node != NULL &&
      (void *)((char *)prev_node + prev_node->block_size) == (void *)free_node) {
    printf("Coalescing free block [%p] with previous block [%p] ...\n", (void *)free_node,
           (void *)prev_node);
    prev_node->block_size += free_node->block_size;
    freelist_node_remove(&fl->head, prev_node, free_node);
  }
}

// Frees previously allocated memory
void *freelist_free(FreeList *fl, void *ptr) {
  if (ptr == NULL) return NULL;

  // Get header and free node pointer
  FreeList_Header *header = (FreeList_Header *)((char *)ptr - sizeof(FreeList_Header));
  FreeList_Node *free_node = (FreeList_Node *)((char *)header - header->padding);
  free_node->block_size = header->block_size;
  free_node->next = NULL;

  FreeList_Node *node = fl->head;
  FreeList_Node *prev_node = NULL;

  // Find correct place to insert in sorted order by address
  while (node != NULL && (void *)node < (void *)free_node) {
    prev_node = node;
    node = node->next;
  }

  // Insert node into free list
  freelist_node_insert(&fl->head, prev_node, free_node);
  fl->used -= free_node->block_size;

  printf("\nBefore coalesce  --> ");
  print_freelist(fl);
  // Merge neighbors if adjacent
  freelist_coalescence(fl, prev_node, free_node);

  printf("\nAfter coalesce  --> ");
  print_freelist(fl);

  return fl;
}

#if defined(FIRST_PLACEMENT)
int main(void) {
  unsigned char buf[1024];  // Increased buffer size to avoid early OOM
  FreeList fl = {0};        // Allocator instance

  free_list_init(&fl, buf, sizeof(buf));
  fl.policy = Placement_Policy_Find_Best;  // or Placement_Policy_Find_Best

  // Allocate int and float as before
  int *i = (int *)freelist_alloc(&fl, sizeof(int), 8);
  float *f = (float *)freelist_alloc(&fl, sizeof(float), 8);

  *i = 100;
  *f = 234.234f;
  printf("[Test1] %d : %f\n", *i, *f);

  // Free float, allocate again to check reuse
  freelist_free(&fl, f);
  float *f2 = (float *)freelist_alloc(&fl, sizeof(float), 8);
  *f2 = 123.456f;
  printf("[Test2] %d : %f\n", *i, *f2);

  // Allocate multiple blocks with different sizes and alignments
  char *c1 = (char *)freelist_alloc(&fl, 16, 16);
  double *d1 = (double *)freelist_alloc(&fl, sizeof(double), 8);
  long long *ll1 = (long long *)freelist_alloc(&fl, sizeof(long long), 16);

  for (int idx = 0; idx < 16; ++idx) c1[idx] = (char)(idx + 65);  // Fill with letters A-P
  *d1 = 3.141592653589793;
  *ll1 = 0x123456789ABCDEF0;

  printf("[Test3] c1: ");
  for (int idx = 0; idx < 16; ++idx) printf("%c", c1[idx]);
  printf(", d1: %.15f, ll1: 0x%llX\n", *d1, *ll1);

  // Free some blocks out of order
  freelist_free(&fl, i);
  freelist_free(&fl, ll1);

  // Allocate a bigger block to check coalescing
  void *big_block = freelist_alloc(&fl, 64, 8);
  if (big_block) {
    printf("[Test4] Allocated big block of 64 bytes at %p\n", big_block);
  } else {
    printf("[Test4] Failed to allocate big block\n");
  }

  // Free remaining blocks
  freelist_free(&fl, f2);
  freelist_free(&fl, d1);
  freelist_free(&fl, c1);
  freelist_free(&fl, big_block);

  // Allocate many small blocks to test fragmentation handling
  void *blocks[10];
  for (int n = 0; n < 10; ++n) {
    blocks[n] = freelist_alloc(&fl, 12, 8);
    printf("[Test5] Allocated small block %d at %p\n", n, blocks[n]);
  }

  // Free every other small block to create fragmentation
  for (int n = 0; n < 10; n += 2) {
    freelist_free(&fl, blocks[n]);
    printf("[Test5] Freed small block %d\n", n);
  }

  // Allocate a medium block to test if it fits into fragmented space
  // Intentionally allocating a non-aligned size
  void *medium_block = freelist_alloc(&fl, 41, 8);
  if (medium_block) {
    printf("[Test5] Allocated medium block at %p\n", medium_block);
  } else {
    printf("[Test5] Failed to allocate medium block\n");
  }

  // Check the allocation of a block of remaining size
  // // Get the remaining block from free list
  // currently we have a block of size 135 bytes left.
  size_t max_alloc_size = 119;
  void *remaining_block = freelist_alloc(&fl, max_alloc_size, 8);

  if (remaining_block == NULL) {
    printf("Allocation FAILED for %zu bytes\n", max_alloc_size);
  } else {
    printf("Allocation succeeded: %zu bytes at %p\n", max_alloc_size, remaining_block);
    printf("Used after allocation: %zu bytes\n", fl.used);
  }

  // Clean up remaining small blocks
  for (int n = 1; n < 10; n += 2) {
    freelist_free(&fl, blocks[n]);
  }
  freelist_free(&fl, medium_block);
  freelist_free(&fl, remaining_block);

  printf("[Test7] Final used bytes: %zu (should be 0)\n", fl.used);

  return 0;
}

#else

int main(void) {
  // 1024-byte buffer
  unsigned char buf[1024];
  FreeList fl = {0};

  // Initialize the free list and set the policy to 'Find_Best'
  free_list_init(&fl, buf, sizeof(buf));
  fl.policy = Placement_Policy_Find_Best;

  // Allocate three blocks of varying sizes
  // This will break the single 512-byte block into smaller free blocks
  void *a = freelist_alloc(&fl, 100, 8);
  void *b = freelist_alloc(&fl, 20, 8);
  void *c = freelist_alloc(&fl, 100, 8);

  // Free the first and third blocks.
  // This creates two free blocks: a larger one and a smaller one.
  freelist_free(&fl, a);
  freelist_free(&fl, c);

  printf("After freeing 'a' and 'c', the free list should have two blocks.\n");
  printf("Block 1 (from 'a') is around 100 bytes + overhead.\n");
  printf("Block 2 (from 'c') is around 100 bytes + overhead.\n");

  // The free blocks are not necessarily in a specific order due to the
  // `freelist_free` function but their size is a known quantity for this test.
  // We have a 100-byte block and another 100-byte block.
  // Let's create an additional small free block in the middle.
  //
  // Current state: Free space `a` (approx 100 bytes), Used space `b` (20
  // bytes), Free space `c` (approx 100 bytes). Let's free 'b' to create a
  // smaller free block between the two 100-byte blocks.
  freelist_free(&fl, b);

  // The free list now has three blocks (due to the `coalescence` function, `a`
  // and `b` may be merged as will `b` and `c` if they are contiguous). Let's
  // create a scenario that is less likely to coalesce.

  // Reset and try a different allocation pattern to create specific free
  // blocks.
  freelist_free_all(&fl);

  // Create three free blocks of specific sizes by allocating and freeing.
  // Allocate three large blocks
  void *p1 = freelist_alloc(&fl, 100, 8);
  void *p2 = freelist_alloc(&fl, 50, 8);
  void *p3 = freelist_alloc(&fl, 150, 8);

  // to prevent unused warning
  (void)p2;

  // Free them in a way that leaves two distinct-sized free blocks
  freelist_free(&fl, p1);  // Leaves a free block of ~112 bytes
  freelist_free(&fl, p3);  // Leaves a free block of ~160 bytes

  // The free list now contains a ~112-byte block and a ~160-byte block.
  // 'p2' (50 bytes) remains allocated in the middle.

  printf("\nTest Case for Best-Fit Policy:\n");
  printf(
      "Free blocks available: ~112 bytes and ~160 bytes (plus a 50-byte "
      "used block in between).\n");

  // Allocate a block of 64 bytes.
  // With First-Fit, it would use the first free block (~112 bytes).
  // With Best-Fit, it should use the first free block as it's the only one that
  // can fit.
  //
  // Let's adjust our test case to make the two free blocks fit.
  freelist_free_all(&fl);

  // Setup a scenario with a 'perfect' fit
  void *alloc1 = freelist_alloc(&fl, 50, 8);  // Used block
  void *alloc2 = freelist_alloc(&fl, 70, 8);  // The 'first' free block candidate
  void *alloc3 = freelist_alloc(&fl, 60, 8);  // Used block
  void *alloc4 = freelist_alloc(&fl, 30, 8);  // The 'best' free block candidate

  freelist_free(&fl, alloc2);  // Free block 1: ~70 bytes
  freelist_free(&fl, alloc4);  // Free block 2: ~30 bytes

  printf(
      "\nCreated two free blocks: a large one (~70 bytes) and a small one "
      "(~30 bytes).\n");
  printf("Now attempting to allocate a 25-byte block.\n");

  // Request a 25-byte block.
  // First-Fit would select the ~70-byte block.
  // Best-Fit should select the ~30-byte block, as it has less fragmentation
  // (smaller 'diff').
  void *best_fit_alloc = freelist_alloc(&fl, 25, 8);

  if (best_fit_alloc) {
    printf("\nSuccessfully allocated 25-byte block at %p.\n", best_fit_alloc);
  } else {
    printf("\nFailed to allocate 25-byte block.\n");
  }

  // Now, let's verify which block was used by looking at the remaining free
  // list. This is difficult to do without a debug function, but the success or
  // failure of subsequent allocations can give clues.

  printf("\nState after best-fit allocation:\n");
  printf("Used: %zu bytes\n", fl.used);

  // Attempt to allocate a block that would only fit in the remaining large
  // block.
  void *large_alloc = freelist_alloc(&fl, 40, 8);
  if (large_alloc) {
    printf("Successfully allocated a 40-byte block.\n");
  } else {
    printf("Failed to allocate a 40-byte block.\n");
  }

  // Clean up
  freelist_free(&fl, alloc1);
  freelist_free(&fl, alloc3);
  freelist_free(&fl, best_fit_alloc);
  freelist_free(&fl, large_alloc);

  printf("[Test6] Final used bytes: %zu (should be 0)\n", fl.used);

  return 0;
}
#endif

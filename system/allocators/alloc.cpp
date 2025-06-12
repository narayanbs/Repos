/**
 * Lecture 6, Essentials of Garbage Collectors class
 *
 * MIT Style License, 2019
 * Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include <assert.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <list>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

/**
 * Machine word.
 */
using word_t = uintptr_t;

/**
 * Machine word alignment. We allocate always by the machine word boundary,
 * doing 4 (32 bit) or 8 (64 bit) bytes alignment.
 *
 * align(3) ->   4/8
 * align(8) ->   8/8
 * align(9) -> 12/16
 */
inline size_t align(size_t x) {
  return (x + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

/**
 * Allocated block of memory. Contains the object header structure,
 * and the actual payload pointer.
 */
struct Block {
  // -------------------------------------
  // 1. Object header

  /**
   * Block size.
   */
  size_t size;

  /**
   * Whether this block is allocated.
   */
  bool used;

  /**
   * Next block.
   */
  Block *next;

  // -------------------------------------
  // 2. User data

  /**
   * Payload pointer.
   * This will guarantee that there will be no gap between the payload and the
   * dynamic data that follows. Since data will be aligned to word_t, the word_t data
   * that follows will also aligned and there will be no gap. Hence allocsize can
   * return a size that is the required size - payload size.
   */
  word_t data[1];
};

/**
 * Mode for searching a free block.
 */
enum class SearchMode {
  FirstFit,
  NextFit,
  BestFit,
  FreeList,
  SegregatedList,
};

/**
 * Heap start. Initialized on first allocation.
 */
static Block *heapStart = nullptr;

/**
 * Current top. Updated on each allocation.
 */
static auto top = heapStart;

/**
 * Previously found block. Updated in `nextFit`.
 */
static Block *searchStart = heapStart;

/**
 * Current search mode.
 */
static auto searchMode = SearchMode::FirstFit;

/**
 * Free list structure. Blocks are added to the free list
 * on the `free` operation. Consequent allocations of the
 * appropriate size reuse the freed blocks.
 */
static std::list<Block *> free_list;

/**
 * Segregated lists.
 */
Block *segregatedLists[] = {
    nullptr,  //   8
    nullptr,  //  16
    nullptr,  //  32
    nullptr,  //  64
    nullptr,  // 128
};

/**
 * Segregated tops.
 */
Block *segregatedTops[] = {
    nullptr,  //   8
    nullptr,  //  16
    nullptr,  //  32
    nullptr,  //  64
    nullptr,  // 128
};

/**
 * Returns total allocation size, reserving in addition the space for
 * the Block structure (object header + first data word).
 *
 * Since the `word_t data[1]` already allocates one word inside the Block
 * structure, we decrease it from the size request: if a user allocates
 * only one word, it's fully in the Block struct.
 */
inline size_t allocSize(size_t size) {
  return sizeof(Block) + size - sizeof(std::declval<Block>().data);
}

/**
 * Requests (maps) memory from OS.
 */
Block *requestFromOS(size_t size) {
  // Current heap break.
  auto block = (Block *)sbrk(0);

  // OOM.
  if (sbrk(allocSize(size)) == (void *)-1) {
    return nullptr;
  }

  return block;
}

/**
 * Splits the block on two, returns the pointer to the smaller sub-block.
 */
Block *split(Block *block, size_t size) {
  auto freePart = (Block *)((char *)block + allocSize(size));
  freePart->size = block->size - size;
  freePart->used = false;
  freePart->next = block->next;

  block->size = size;
  block->next = freePart;

  if (searchMode == SearchMode::FreeList) {
    free_list.push_back(block);
  }

  return block;
}

/**
 * Whether this block can be split.
 */
inline bool canSplit(Block *block, size_t size) {
  return (int)(allocSize(block->size) - size) >= (int)sizeof(Block);
}

/**
 * Allocates a block from the list, splitting if needed.
 */
Block *listAllocate(Block *block, size_t size) {
  // Split the larger block, reusing the free part.
  if (searchMode != SearchMode::SegregatedList && canSplit(block, size)) {
    block = split(block, size);
  }

  block->used = true;
  block->size = size;

  return block;
}

/**
 * First-fit algorithm.
 */
Block *firstFit(size_t size) {
  auto block = heapStart;

  while (block != nullptr) {
    // O(n) search.
    if (block->used || block->size < size) {
      block = block->next;
      continue;
    }

    // Found the block:
    return listAllocate(block, size);
  }

  return nullptr;
}

/**
 * Next-fit algorithm.
 */
Block *nextFit(size_t size) {
  // Prime the search start.
  if (searchStart == nullptr) {
    searchStart = heapStart;
  }

  auto start = searchStart;
  auto block = start;

  while (block != nullptr) {
    // O(n) search.
    if (block->used || block->size < size) {
      block = block->next;
      // Start from the beginning.
      if (block == nullptr) {
        block = heapStart;
      }
      // Did the full circle, and didn't find.
      if (block == start) {
        break;
      }
      continue;
    }

    // Found the block:
    searchStart = block;
    return listAllocate(block, size);
  }

  return nullptr;
}

/**
 * Best-fit algorithm.
 */
Block *bestFit(size_t size) {
  auto block = heapStart;
  Block *best = nullptr;

  while (block != nullptr) {
    if (block->used || block->size < size) {
      block = block->next;
      continue;
    }
    // Found a block of a smaller size, than previous best:
    if (best == nullptr || block->size < best->size) {
      best = block;
      block = block->next;
    }
  }

  if (best == nullptr) {
    return nullptr;
  }

  return listAllocate(best, size);
}

/**
 * Explicit free-list algorithm.
 */
Block *freeList(size_t size) {
  for (const auto &block : free_list) {
    if (block->size < size) {
      continue;
    }
    free_list.remove(block);
    return listAllocate(block, size);
  }
  return nullptr;
}

/**
 * Gets bucket number from segregatedLists
 * based on the size.
 */
inline int getBucket(size_t size) {
  return size / sizeof(word_t) - 1;
}

/**
 * Segregated fit algorithm.
 */
Block *segregatedFit(size_t size) {
  // Bucket number based on size.
  auto bucket = getBucket(size);
  auto originalHeapStart = heapStart;

  // Init the search.
  heapStart = segregatedLists[bucket];

  // Use first-fit here, but can be any:
  auto block = firstFit(size);

  heapStart = originalHeapStart;
  return block;
}

/**
 * Tries to find a block that fits.
 */
Block *findBlock(size_t size) {
  switch (searchMode) {
    case SearchMode::FirstFit:
      return firstFit(size);
    case SearchMode::NextFit:
      return nextFit(size);
    case SearchMode::BestFit:
      return bestFit(size);
    case SearchMode::FreeList:
      return freeList(size);
    case SearchMode::SegregatedList:
      return segregatedFit(size);
  }
}

/**
 * Coalesces two adjacent blocks.
 */
Block *coalesce(Block *block) {
  if (!block->next->used) {
    if (block->next == top) {
      top = block;
    }

    block->size += block->next->size;
    block->next = block->next->next;
  }
  return block;
}

/**
 * Whether we should merge this block.
 */
bool canCoalesce(Block *block) {
  return block->next && !block->next->used;
}

/**
 * Returns object header.
 */
Block *getHeader(word_t *data) {
  return (Block *)((char *)data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

/**
 * Reset the heap to the original position.
 */
void resetHeap() {
  // Already reset.
  if (heapStart == nullptr) {
    return;
  }

  // Roll back to the beginning.
  brk(heapStart);

  heapStart = nullptr;
  top = nullptr;
  searchStart = nullptr;
}

/**
 * Initializes the heap and the search mode.
 */
void init(SearchMode mode) {
  searchMode = mode;
  resetHeap();
}

/**
 * Allocates a block of memory of (at least) `size` bytes.
 */
word_t *alloc(size_t size) {
  size = align(size);

  // ---------------------------------------------------------
  // 1. Search for a free block in the free-list:
  //
  // Traverse the blocks list, searching for a block of
  // the appropriate size

  if (auto block = findBlock(size)) {
    return block->data;
  }

  // ---------------------------------------------------------
  // 2. If block not found in the free list, request from OS:

  // No block found, request to map more memory from the OS,
  // bumping the program break (brk).
  auto block = requestFromOS(size);

  // Set the size:
  block->size = size;
  block->used = true;

  if (searchMode == SearchMode::SegregatedList) {
    auto bucket = getBucket(size);
    // Init bucket.
    if (segregatedLists[bucket] == nullptr) {
      segregatedLists[bucket] = block;
    }
    // Chain the blocks in the bucket.
    if (segregatedTops[bucket] != nullptr) {
      segregatedTops[bucket]->next = block;
    }
    segregatedTops[bucket] = block;
  } else {
    // Init heap.
    if (heapStart == nullptr) {
      heapStart = block;
    }
    // Chain the blocks.
    if (top != nullptr) {
      top->next = block;
    }
    top = block;
  }

  // User payload:
  return block->data;
}

/**
 * Frees the previously allocated block.
 */
void free(word_t *data) {
  auto block = getHeader(data);
  if (searchMode != SearchMode::SegregatedList && canCoalesce(block)) {
    block = coalesce(block);
  }
  block->used = false;
  if (searchMode == SearchMode::FreeList) {
    free_list.push_back(block);
  }
}

void visit(const std::function<void(Block *)> &callback) {
  auto block = heapStart;
  while (block != nullptr) {
    callback(block);
    block = block->next;
  }
}

/**
 * Traverses the heap.
 */
void segregatedTraverse(const std::function<void(Block *)> &callback) {
  for (const auto &block : segregatedLists) {
    auto originalHeapStart = heapStart;
    heapStart = block;
    visit(callback);
    heapStart = originalHeapStart;
  }
}

/**
 * Traverses the heap.
 */
void traverse(const std::function<void(Block *)> &callback) {
  if (searchMode == SearchMode::SegregatedList) {
    return segregatedTraverse(callback);
  }
  visit(callback);
}

void printBlocks() {
  traverse([](Block *block) { std::cout << "[" << block->size << ", " << block->used << "] "; });
  std::cout << "\n";
}

int main(int argc, char const *argv[]) {
  // ===========================================================================
  // First-fit search

  std::cout << "=======================================================\n";
  std::cout << "# First-fit search\n\n";

  init(SearchMode::FirstFit);

  // --------------------------------------
  // Test case 1: Alignment
  //
  // A request for 3 bytes is aligned to 8.
  //

  auto p1 = alloc(3);
  auto p1b = getHeader(p1);
  assert(p1b->size == sizeof(word_t));

  printBlocks();

  // --------------------------------------
  // Test case 2: Exact amount of aligned bytes
  //

  auto p2 = alloc(8);
  auto p2b = getHeader(p2);
  assert(p2b->size == 8);

  printBlocks();

  // --------------------------------------
  // Test case 3: Free the object
  //
  // The pointer to the reclaimed block is added
  // to the free list.
  //

  free(p2);
  assert(p2b->used == false);

  printBlocks();

  // --------------------------------------
  // Test case 4: The block is reused
  //
  // A consequent allocation of the same size reuses
  // the previously freed block.
  //

  auto p3 = alloc(8);
  auto p3b = getHeader(p3);
  assert(p3b->size == 8);
  assert(p3b == p2b);  // Reused!

  printBlocks();

  auto p4 = alloc(8);
  auto p4b = getHeader(p4);
  assert(p4b->size == 8);

  printBlocks();

  auto p5 = alloc(8);
  assert(getHeader(p5)->size == 8);

  printBlocks();

  free(p5);

  printBlocks();

  // This free coalesces with p5 block.
  free(p4);

  // Only one free block with size 16.
  assert(getHeader(p4)->size == 16);

  printBlocks();

  auto p6 = alloc(16);
  auto p6b = getHeader(p6);
  assert(p6b == p4b);  // Reused!
  assert(p6b->size == 16);

  printBlocks();

  auto p7 = alloc(128);
  auto p7b = getHeader(p7);
  assert(p7b->size == 128);

  printBlocks();

  free(p7);

  printBlocks();

  auto p8 = alloc(8);
  auto p8b = getHeader(p8);
  assert(p8b == p7b);
  assert(p8b->size == 8);

  printBlocks();

  // ===========================================================================
  // Next-fit search

  std::cout << "\n=======================================================\n";
  std::cout << "# Next-fit search\n\n";

  init(SearchMode::NextFit);

  // --------------------------------------
  // Test case 5: Next search start position
  //

  // [[8, 1], [8, 1], [8, 1]]
  alloc(8);
  alloc(8);
  alloc(8);
  printBlocks();

  // [[8, 1], [8, 1], [8, 1], [16, 1], [16, 1]]
  auto o1 = alloc(16);
  auto o2 = alloc(16);
  printBlocks();

  // [[8, 1], [8, 1], [8, 1], [16, 0], [16, 0]]
  free(o1);
  free(o2);
  printBlocks();

  // [[8, 1], [8, 1], [8, 1], [16, 1], [16, 0]]
  auto o3 = alloc(16);
  printBlocks();

  // Start position from o3:
  assert(searchStart == getHeader(o3));

  // [[8, 1], [8, 1], [8, 1], [16, 1], [16, 1]]
  //                           ^ start here
  alloc(16);
  printBlocks();

  // ===========================================================================
  // Best-fit search

  std::cout << "\n=======================================================\n";
  std::cout << "# Best-fit search\n\n";

  init(SearchMode::BestFit);

  // --------------------------------------
  // Test case 6: Best-fit search
  //

  // [[8, 1], [64, 1], [8, 1], [16, 1]]
  alloc(8);
  auto z1 = alloc(64);
  alloc(8);
  auto z2 = alloc(16);
  printBlocks();

  // Free the last 16
  free(z2);

  // Free 64:
  free(z1);

  // [[8, 1], [64, 0], [8, 1], [16, 0]]
  printBlocks();

  // Reuse the last 16 block:
  auto z3 = alloc(16);
  assert(getHeader(z3) == getHeader(z2));

  // [[8, 1], [64, 0], [8, 1], [16, 1]]
  printBlocks();

  // Reuse 64, splitting it to 16, and 48
  z3 = alloc(16);
  assert(getHeader(z3) == getHeader(z1));

  // [[8, 1], [16, 1], [48, 0], [8, 1], [16, 1]]
  printBlocks();

  // ===========================================================================
  // Free-list search

  std::cout << "\n=======================================================\n";
  std::cout << "# Free-list search\n\n";

  init(SearchMode::FreeList);

  alloc(8);
  alloc(8);
  auto v1 = alloc(16);
  alloc(8);
  printBlocks();

  free(v1);
  assert(free_list.size() == 1);
  printBlocks();

  auto v2 = alloc(16);
  assert(free_list.size() == 0);
  assert(getHeader(v1) == getHeader(v2));
  printBlocks();

  // ===========================================================================
  // Segregated-fit search

  std::cout << "\n=======================================================\n";
  std::cout << "# Segregated-list search\n\n";

  init(SearchMode::SegregatedList);

  auto s1 = alloc(3);
  auto s2 = alloc(8);

  assert(getHeader(s1) == segregatedLists[0]);
  assert(getHeader(s2) == segregatedLists[0]->next);

  printBlocks();

  auto s3 = alloc(16);
  assert(getHeader(s3) == segregatedLists[1]);
  printBlocks();

  auto s4 = alloc(8);
  assert(getHeader(s4) == segregatedLists[0]->next->next);
  printBlocks();

  auto s5 = alloc(32);
  assert(getHeader(s5) == segregatedLists[3]);
  printBlocks();

  free(s1);
  free(s2);
  free(s3);

  printBlocks();

  puts("\nAll assertions passed!\n");

  return 0;
}

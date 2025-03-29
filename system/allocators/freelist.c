#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct FreeList_Node FreeList_Node;

struct FreeList_Node {
  FreeList_Node *next;
  size_t block_size;
};

typedef struct FreeList_Header FreeList_Header;

struct FreeList_Header {
  size_t block_size;
  size_t padding;
};

typedef enum Placement_Policy Placement_Policy;

enum Placement_Policy { Placement_Policy_Find_First, Placement_Policy_Find_Best };

typedef struct FreeList FreeList;

struct FreeList {
  void *data;
  size_t size;
  size_t used;

  FreeList_Node *head;
  Placement_Policy policy;
};

void freelist_free_all(FreeList *fl) {
  fl->used = 0;
  FreeList_Node *firstnode = (FreeList_Node *)fl->data;
  firstnode->block_size = fl->size;
  firstnode->next = NULL;
  fl->head = firstnode;
}

void free_list_init(FreeList *fl, void *data, size_t size) {
  fl->data = data;
  fl->size = size;
  freelist_free_all(fl);
}

bool is_power_of_two(uintptr_t x) {
  return (x & (x - 1)) == 0;
}

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
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

FreeList_Node *freelist_find_first(FreeList *fl, size_t size, size_t alignment, size_t *padding_,
                                   FreeList_Node **prev_node_) {
  FreeList_Node *node = fl->head;
  FreeList_Node *prev_node = NULL;

  size_t padding = 0;

  while (node != NULL) {
    padding = calc_padding_with_header((uintptr_t)node, (uintptr_t)alignment, sizeof(FreeList_Header));
    size_t required_space = size + padding;
    if (node->block_size >= required_space) {
      break;
    }
    prev_node = node;
    node = node->next;
  }
  if (padding_) *padding_ = padding;
  if (prev_node_) *prev_node_ = prev_node;
  return node;
}

FreeList_Node *freelist_find_best(FreeList *fl, size_t size, size_t alignment, size_t *padding_,
                                  FreeList_Node **prev_node_) {
  // This iterates the entire list to find the best fit
  // O(n)
  size_t smallest_diff = ~(size_t)0;

  FreeList_Node *node = fl->head;
  FreeList_Node *prev_node = NULL;
  FreeList_Node *best_node = NULL;

  size_t padding = 0;

  while (node != NULL) {
    padding = calc_padding_with_header((uintptr_t)node, (uintptr_t)alignment, sizeof(FreeList_Header));
    size_t required_space = size + padding;
    if (node->block_size >= required_space && (node->block_size - required_space < smallest_diff)) {
      best_node = node;
      smallest_diff = node->block_size - required_space;
    }
    prev_node = node;
    node = node->next;
  }
  if (padding_) *padding_ = padding;
  if (prev_node_) *prev_node_ = prev_node;
  return best_node;
}

void freelist_node_insert(FreeList_Node **phead, FreeList_Node *prev_node, FreeList_Node *new_node) {
  if (prev_node == NULL) {
    if (*phead != NULL) {
      new_node->next = *phead;
    } else {
      *phead = new_node;
    }
  } else {
    if (prev_node->next == NULL) {
      prev_node->next = new_node;
      new_node->next = NULL;
    } else {
      new_node->next = prev_node->next;
      prev_node->next = new_node;
    }
  }
}

void freelist_node_remove(FreeList_Node **phead, FreeList_Node *prev_node, FreeList_Node *del_node) {
  if (prev_node == NULL) {
    *phead = del_node->next;
  } else {
    prev_node->next = del_node->next;
  }
}

void *freelist_alloc(FreeList *fl, size_t size, size_t alignment) {
  size_t padding = 0;
  FreeList_Node *prev_node = NULL;
  FreeList_Node *node = NULL;
  size_t alignment_padding, required_space, remaining;
  FreeList_Header *header_ptr;

  if (size < sizeof(FreeList_Node)) {
    size = sizeof(FreeList_Node);
  }
  if (alignment < 8) {
    alignment = 8;
  }

  if (fl->policy == Placement_Policy_Find_Best) {
    node = freelist_find_best(fl, size, alignment, &padding, &prev_node);
  } else {
    node = freelist_find_first(fl, size, alignment, &padding, &prev_node);
  }
  if (node == NULL) {
    assert(0 && "Free list has no free memory");
    return NULL;
  }

  alignment_padding = padding - sizeof(FreeList_Header);
  required_space = size + padding;
  remaining = node->block_size - required_space;

  if (remaining > 0) {
    FreeList_Node *new_node = (FreeList_Node *)((char *)node + required_space);
    new_node->block_size = remaining;
    freelist_node_insert(&fl->head, node, new_node);
  }

  freelist_node_remove(&fl->head, prev_node, node);

  header_ptr = (FreeList_Header *)((char *)node + alignment_padding);
  header_ptr->block_size = required_space;
  header_ptr->padding = alignment_padding;

  fl->used += required_space;

  return (void *)((char *)header_ptr + sizeof(FreeList_Header));
}

void freelist_coalescence(FreeList *fl, FreeList_Node *prev_node, FreeList_Node *free_node) {
  if (free_node->next != NULL && (void *)((char *)free_node + free_node->block_size) == free_node->next) {
    free_node->block_size += free_node->next->block_size;
    freelist_node_remove(&fl->head, free_node, free_node->next);
  }

  if (prev_node->next != NULL && (void *)((char *)prev_node + prev_node->block_size) == free_node) {
    prev_node->block_size += free_node->next->block_size;
    freelist_node_remove(&fl->head, prev_node, free_node);
  }
}

void *freelist_free(FreeList *fl, void *ptr) {
  FreeList_Header *header;
  FreeList_Node *free_node;
  FreeList_Node *node;
  FreeList_Node *prev_node = NULL;

  if (ptr == NULL) {
    return NULL;
  }

  header = (FreeList_Header *)((char *)ptr - sizeof(FreeList_Header));
  free_node = (FreeList_Node *)header;
  free_node->block_size = header->block_size + header->padding;
  free_node->next = NULL;

  node = fl->head;
  while (node != NULL) {
    if ((FreeList_Node *)ptr < node) {
      freelist_node_insert(&fl->head, prev_node, free_node);
      break;
    }
    prev_node = node;
    node = node->next;
  }

  fl->used -= free_node->block_size;

  freelist_coalescence(fl, prev_node, free_node);
  return fl;
}

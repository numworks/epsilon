#include "tree_pool.h"
#include <string.h>
#include "tree_node.h"

void * TreePool::alloc(size_t size) {
  if (m_cursor >= m_buffer + BufferSize || m_cursor + size > m_buffer + BufferSize) {
    return nullptr;
  }
  void * result = m_cursor;
  m_cursor += size;
  return result;
}

void TreePool::dealloc(void * ptr) {
  assert(ptr >= m_buffer && ptr < m_cursor);
  TreeNode * node = reinterpret_cast<TreeNode *>(ptr);
  size_t size = node->size();
  printf("Dealloc %d(%p) of size %zu\n", node->m_identifier, node, size);

  // Step 1 - Compact the pool
  memmove(
    ptr,
    static_cast<char *>(ptr) + size,
    m_cursor - (static_cast<char *>(ptr) + size)
  );
  m_cursor -= size;

  // Step 2 - Update m_nodeForIdentifier
  for (int i = 0; i < MaxNumberOfNodes; i++) {
    if (m_nodeForIdentifier[i] > node) {
      m_nodeForIdentifier[i] -= size;
    }
  }
}

TreeNode * TreePool::node(int identifier) const {
  assert(identifier >= 0 && identifier <= MaxNumberOfNodes);
  return m_nodeForIdentifier[identifier];
}

static inline void swap(uint32_t * a, uint32_t * b) {
  uint32_t tmp = *a;
  *a = *b;
  *b = tmp;
}

static inline void insert(char * source, char * destination, size_t length) {
  assert(length % 4 == 0);
  assert((long)source % 4 == 0);
  assert((long)destination % 4 == 0);
  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = length/4;

  if (dst < src) {
    if (src - dst <= len) {
      uint32_t * srcPointer = src;
      uint32_t * dstPointer = dst;
      while (dstPointer != src) {
        swap(srcPointer, dstPointer);
        srcPointer++;
        dstPointer++;
        if (srcPointer == src + len) {
          srcPointer = src;
        }
      }
    }
  } else {
    assert(false); // TODO: Implement this case
  }
}

void TreePool::move(TreeNode * source, TreeNode * destination) {
  if (source == destination) {
    return;
  }
  insert(reinterpret_cast<char *>(source), reinterpret_cast<char *>(destination), source->deepSize());
  // Here, update the nodeForIdentifier array
}

#if TREE_LOGGING
#include <stdio.h>

void TreePool::log() {
  printf("POOL:");
  for (TreeNode * node : *this) {
    printf("|(%03d|%s|%03d|%p)", node->m_identifier, node->description(), node->retainCount(), node);
  }
  printf("|\n");
}
#endif


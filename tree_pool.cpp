#include "tree_pool.h"
#include <string.h>
#include "tree_node.h"

void * TreePool::alloc(size_t size) {
  void * result = m_cursor;
  m_cursor += size;
  return result;
}

void TreePool::dealloc(void * ptr, size_t size) {
  // Step 1 - Compact the pool
  memmove(
    ptr,
    static_cast<char *>(ptr) - size,
    m_cursor - static_cast<char *>(ptr)
  );
  m_cursor -= size;
}

TreeNode * TreePool::node(int identifier) const {
  for (TreeNode * node : *this) {
    if (node->identifier() == identifier) {
      return node;
    }
  }
  /*
  TreeNode * node = const_cast<TreeNode *>(reinterpret_cast<const TreeNode *>(m_buffer));
  TreeNode * endOfPool = reinterpret_cast<TreeNode *>(m_cursor);
  while (node < endOfPool) {
    if (node->identifier() == identifier) {
      return node;
    }
    node = node->next();
  }
  */
  return nullptr;
}

#if TREE_LOGGING
#include <stdio.h>

void TreePool::log() {
  printf("POOL:");
  for (TreeNode * node : *this) {
    printf("|%d", node->m_identifier);
    //printf("|(%03d|%s|%03d)", node->m_identifier, node->description(), node->retainCount());
  }
  printf("|\n");
}
#endif


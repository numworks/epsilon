#include "tree_pool.h"
#include "tree_node.h"
#include <string.h>

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

static void memmove32(uint32_t * dst, uint32_t * src, size_t len) {
  if (dst > src) {
    src += len;
    dst += len;
    while (len--) {
      *--dst = *--src;
    }
  } else {
    while (len--) {
      *dst++ = *src++;
    }
  }
}

void TreePool::insert(char * destination, char * source, size_t length) {
  if (source == destination || destination < source + length) {
    return;
  }

  assert(length % 4 == 0);
  assert((long)source % 4 == 0);
  assert((long)destination % 4 == 0);

  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = length/4;
  char tempBuffer[BufferSize];
  uint32_t * tmp = reinterpret_cast<uint32_t *>(tempBuffer);
  memmove32(reinterpret_cast<uint32_t *>(tmp), src, len);
  if (dst < src) {
    memmove32(dst + len, dst, src - dst);
    memmove32(dst, tmp, len);
  } else {
    memmove32(src, src + len, dst - (src + len));
    memmove32(dst - len, tmp, len);
  }
}

void TreePool::move(TreeNode * source, TreeNode * destination) {
  if (source == destination) {
    return;
  }
  // Move the Node
  size_t srcDeepSize = source->deepSize();
  insert(reinterpret_cast<char *>(destination), reinterpret_cast<char *>(source), srcDeepSize);

  // Update the nodeForIdentifier array
  for (int i = 0; i < MaxNumberOfNodes; i++) {
    void * nodeAddress = m_nodeForIdentifier[i];
    if (nodeAddress == nullptr) {
      continue;
    } else if (nodeAddress >= source && nodeAddress < source + srcDeepSize) {
      if (destination < source) {
        m_nodeForIdentifier[i] -= (source - destination);
      } else {
        m_nodeForIdentifier[i] += destination - (source + srcDeepSize);
      }
    } else if (nodeAddress > source && nodeAddress < destination) {
      m_nodeForIdentifier[i] -= srcDeepSize;
    } else if (nodeAddress < source && nodeAddress > destination) {
      m_nodeForIdentifier[i] += srcDeepSize;
    }
  }
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


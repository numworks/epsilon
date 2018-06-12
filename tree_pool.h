#ifndef TREE_POOL_H
#define TREE_POOL_H

#include <stddef.h>

#include "tree_node.h"

class TreePool {
public:
  TreePool() : m_lastIdentifier(0), m_cursor(m_buffer) {}

  int generateIdentifier() {
    /* For now we're not bothering with making sure the identifiers are indeed
     * unique. We're just assuming we'll never overflow... */
    //assert(node(m_lastIdentifier) == nullptr);
    return m_lastIdentifier++;
  }
  void reclaimIdentifier(int identifier) {
  }

  void * alloc(size_t size);
  void dealloc(void * ptr);

  TreeNode * node(int identifier) const;
  void move(TreeNode * source, TreeNode * destination);
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  TreeNode * deepCopy(TreeNode * node) {
    size_t size = node->deepSize();
    void * ptr = alloc(size);
    memcpy(ptr, static_cast<void *>(node), size);
    TreeNode * copy = reinterpret_cast<TreeNode *>(ptr);
    copy->rename(generateIdentifier());
    for (TreeNode * child : copy->depthFirstChildren()) {
      child->rename(generateIdentifier());
    }
    return copy;
  }

#if TREE_LOGGING
  void log();
#endif

private:

  TreeNode::DepthFirst::Iterator begin() const { return TreeNode::DepthFirst::Iterator(first()); }
  TreeNode::DepthFirst::Iterator end() const { return TreeNode::DepthFirst::Iterator(last()); }

  int m_lastIdentifier;
  char * m_cursor;
  char m_buffer[256];
};

#endif

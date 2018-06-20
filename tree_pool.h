#ifndef TREE_POOL_H
#define TREE_POOL_H

#include <stddef.h>

#include "tree_node.h"

class TreePool {
public:
  TreePool() : m_cursor(m_buffer) { }

  template <class T>
  TreeNode * createTreeNode() {
    // Find a new identifier
    // Find a memory location for node
  }
  void discardTreeNode(TreeNode * node) {
    // Reclaim node's identifier
    // then dealloc node's memory
    // Then call the destructor on node
  }

  int generateIdentifier() {
    int newIdentifier = -1;
    for (int i = 0; i < MaxNumberOfNodes; i++) {
      if (m_nodeForIdentifier[i] == nullptr) {
        newIdentifier = i;
        break;
      }
    }
    printf("Generating identifier %d\n", newIdentifier);
    return newIdentifier;
  }

  void freeIdentifier(int identifier) {
    assert(identifier >= 0 && identifier < MaxNumberOfNodes);
    m_nodeForIdentifier[identifier] = nullptr;
  }

  void * alloc(size_t size);
  void dealloc(void * ptr);

  TreeNode * node(int identifier) const;
  void move(TreeNode * source, TreeNode * destination);
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  void registerNode(TreeNode * node) {
    m_nodeForIdentifier[node->identifier()] = node;
  }

  void renameNode(TreeNode * node) {
    node->rename(generateIdentifier());
    registerNode(node);
  }

  TreeNode * deepCopy(TreeNode * node) {
    size_t size = node->deepSize();
    void * ptr = alloc(size);
    memcpy(ptr, static_cast<void *>(node), size);
    TreeNode * copy = reinterpret_cast<TreeNode *>(ptr);
    renameNode(copy);
    for (TreeNode * child : copy->depthFirstChildren()) {
      renameNode(child);
    }
    return copy;
  }

#if TREE_LOGGING
  void log();
#endif

private:

  TreeNode::DepthFirst::Iterator begin() const { return TreeNode::DepthFirst::Iterator(first()); }
  TreeNode::DepthFirst::Iterator end() const { return TreeNode::DepthFirst::Iterator(last()); }

  char * m_cursor;

  constexpr static int BufferSize = 256;
  char m_buffer[BufferSize];
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
};

#endif

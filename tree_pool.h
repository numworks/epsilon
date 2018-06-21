#ifndef TREE_POOL_H
#define TREE_POOL_H

#include <stddef.h>
#include "tree_node.h"

class TreePool {
public:
  static TreePool * sharedPool();

  // Identifiers
  int generateIdentifier() {
    int newIdentifier = -1;
    for (int i = 0; i < MaxNumberOfNodes; i++) {
      if (m_nodeForIdentifier[i] == nullptr) {
        newIdentifier = i;
        break;
      }
    }
    printf("Generating identifier %d\n", newIdentifier);
    assert(newIdentifier != -1); // TODO error handling
    return newIdentifier;
  }

  void freeIdentifier(int identifier) {
    assert(identifier >= 0 && identifier < MaxNumberOfNodes);
    printf("DELETE IDENTIFIER %d\n", identifier);
    m_nodeForIdentifier[identifier] = nullptr;
  }

  // Pool memory
  void * alloc(size_t size);
  void dealloc(void * ptr);

  // Node
  template <class T>
  TreeNode * createTreeNode() {
    // TODO
    // Find a new identifier
    // Find a memory location for node
  }
  void discardTreeNode(TreeNode * node) {
    // TODO
    // Reclaim node's identifier
    // Then call the destructor on node
    // then dealloc node's memory
  }
  TreeNode * node(int identifier) const;
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  void logNodeForIdentifierArray();

  void move(TreeNode * source, TreeNode * destination);

  void registerNode(TreeNode * node) {
    printf("Registering identifier %d with node %p\n", node->identifier(), node);
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
  TreePool() : m_cursor(m_buffer) { }
  static inline void insert(char * destination, char * source, size_t length);
  TreeNode::DepthFirst::Iterator begin() const { return TreeNode::DepthFirst::Iterator(first()); }
  TreeNode::DepthFirst::Iterator end() const { return TreeNode::DepthFirst::Iterator(last()); }

  char * m_cursor;

  constexpr static int BufferSize = 256;
  char m_buffer[BufferSize];
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
};

#endif

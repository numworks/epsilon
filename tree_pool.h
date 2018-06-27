#ifndef TREE_POOL_H
#define TREE_POOL_H

#include <stddef.h>
#include "tree_node.h"
#include <new>

class TreePool {
  friend class TreeNode;
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
    assert(newIdentifier != -1); // TODO error handling
    return newIdentifier;
  }

  void freeIdentifier(int identifier) {
    assert(identifier >= 0 && identifier < MaxNumberOfNodes);
    m_nodeForIdentifier[identifier] = nullptr;
  }

  // Pool memory
  void * alloc(size_t size);
  void dealloc(void * ptr);

  // Node
  template <typename T>
  T * createTreeNode() {
    int nodeIdentifier = generateIdentifier();
    if (nodeIdentifier == -1) {
      return nullptr; // TODO return static node "failedAllocation"
    }
    void * ptr = alloc(sizeof(T));
    // TODO handle allocation problem!
    T * node = new(ptr) T();
    node->rename(nodeIdentifier);
    registerNode(node);
    return node;
  }

  void discardTreeNode(TreeNode * node) {
    int nodeIdentifier = node->identifier();
    node->~TreeNode();
    dealloc(static_cast<void *>(node));
    freeIdentifier(nodeIdentifier);
  }

  TreeNode * node(int identifier) const;
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  void logNodeForIdentifierArray();

  void move(TreeNode * source, TreeNode * destination);

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

  // Debug
  int numberOfNodes() const {
    int count = 0;
    AllPool nodes = const_cast<TreePool *>(this)->allNodes();
    for (TreeNode * t : nodes) {
      count++;
    }
    return count;
  }

protected:
  constexpr static int BufferSize = 256;
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);

  class AllPool {
  public:
    AllPool(TreeNode * node) : m_node(node) {}
    class Iterator : public TreeNode::Iterator {
    public:
      using TreeNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node); }
    Iterator end() const { return Iterator(TreePool::sharedPool()->last()); }
  private:
    TreeNode * m_node;
  };
  AllPool allNodes() { return AllPool(*(begin())); }

  class Roots {
  public:
    Roots(TreeNode * node) : m_node(node) {}
    class Iterator : public TreeNode::Iterator {
    public:
      using TreeNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = m_node->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node); }
    Iterator end() const { return Iterator(TreePool::sharedPool()->last()); }
  private:
    TreeNode * m_node;
  };
  Roots roots() { return Roots(*(begin())); }

  TreeNode::DepthFirst::Iterator begin() const { return TreeNode::DepthFirst::Iterator(first()); }
  TreeNode::DepthFirst::Iterator end() const { return TreeNode::DepthFirst::Iterator(last()); }

private:
  TreePool() : m_cursor(m_buffer) { }
  static inline bool insert(char * destination, char * source, size_t length);

  char * m_cursor;
  char m_buffer[BufferSize];
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
};

#endif

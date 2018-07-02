#ifndef TREE_POOL_H
#define TREE_POOL_H

#include "tree_node.h"
#include <stddef.h>
#include <string.h>
#include <new>

class TreePool {
  friend class TreeNode;
public:
  static TreePool * sharedPool();

  // Node
  TreeNode * node(int identifier) const;
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  template <typename T>
  TreeNode * createTreeNode() {
    int nodeIdentifier = generateIdentifier();
    if (nodeIdentifier == -1) {
      T::FailedAllocationStaticNode()->retain();
      return T::FailedAllocationStaticNode();
    }
    void * ptr = alloc(sizeof(T));
    if (ptr == nullptr) {
      T::FailedAllocationStaticNode()->retain();
      return T::FailedAllocationStaticNode();
    }
    T * node = new(ptr) T();
    node->rename(nodeIdentifier);
    registerNode(node);
    return node;
  }

  void move(TreeNode * source, TreeNode * destination);
  void moveChildren(TreeNode * sourceParent, TreeNode * destination);

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

  int registerStaticNode(TreeNode * node) {
    int nodeID = 0;
    while (m_staticNodes[nodeID] != nullptr && nodeID < MaxNumberOfStaticNodes) {
      nodeID++;
    }
    assert(nodeID < MaxNumberOfStaticNodes);
    m_staticNodes[nodeID] = node;
    return identifierOfStaticNodeAtIndex(nodeID);
  }

  // Debug
  void log();

  int numberOfNodes() const {
    int count = 0;
    AllPool nodes = const_cast<TreePool *>(this)->allNodes();
    for (TreeNode * t : nodes) {
      count++;
    }
    return count;
  }

private:
  constexpr static int BufferSize = 256;
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);
  constexpr static int MaxNumberOfStaticNodes = 2;

  // TreeNode
  void discardTreeNode(TreeNode * node) {
    int nodeIdentifier = node->identifier();
    node->~TreeNode();
    dealloc(node);
    freeIdentifier(nodeIdentifier);
  }
  void registerNode(TreeNode * node) {
    m_nodeForIdentifier[node->identifier()] = node;
  }

  void renameNode(TreeNode * node) {
    node->rename(generateIdentifier());
    registerNode(node);
  }

  int identifierOfStaticNodeAtIndex(int index) const { return - (index+2);} // We do not want positive indexes that are reserved for pool nodes, and -1 is reserved for node initialisation.
  int indexOfStaticNode(int id) const { return -id-2;}

  // Iterators

  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }

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

  TreePool() : m_cursor(m_buffer) { }

  // Pool memory
  void * alloc(size_t size);
  void dealloc(TreeNode * ptr);
  bool insert(char * destination, char * source, size_t length);
  void moveNodes(TreeNode * source, TreeNode * destination, size_t moveLength);

  // Identifiers
  int generateIdentifier() {
    int newIdentifier = -1;
    for (int i = 0; i < MaxNumberOfNodes; i++) {
      if (m_nodeForIdentifier[i] == nullptr) {
        newIdentifier = i;
        break;
      }
    }
    assert(newIdentifier != -1);
    return newIdentifier;
  }

  void freeIdentifier(int identifier) {
    assert(identifier >= 0 && identifier < MaxNumberOfNodes);
    m_nodeForIdentifier[identifier] = nullptr;
  }

  // Debug
  void logNodeForIdentifierArray();

  char * m_cursor;
  char m_buffer[BufferSize];
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
  TreeNode * m_staticNodes[MaxNumberOfStaticNodes];
};

#endif

#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include "tree_node.h"
#include <poincare/ghost_node.h>
#include <stddef.h>
#include <string.h>
#include <new>
#if POINCARE_TREE_LOG
#include <ostream>
#include <iostream>
#endif

namespace Poincare {

class TreeHandle;

class TreePool final {
  friend class TreeNode;
  friend class TreeHandle;
  friend class ExceptionCheckpoint;
public:
  static TreePool * sharedPool() { assert(SharedStaticPool != nullptr); return SharedStaticPool; }
  static void RegisterPool(TreePool * pool) {  assert(SharedStaticPool == nullptr); SharedStaticPool = pool; }

  TreePool() : m_cursor(buffer()) {}

  // Node
  TreeNode * node(int identifier) const {
    assert(identifier >= 0 && identifier <= MaxNumberOfNodes);
    return m_nodeForIdentifier[identifier];
  }

  // Pool memory
  void * alloc(size_t size);
  void move(TreeNode * destination, TreeNode * source, int realNumberOfSourceChildren);
  void moveChildren(TreeNode * destination, TreeNode * sourceParent);
  void removeChildren(TreeNode * node, int nodeNumberOfChildren);
  void removeChildrenAndDestroy(TreeNode * nodeToDestroy, int nodeNumberOfChildren);

  TreeNode * deepCopy(TreeNode * node);
  TreeNode * copyTreeFromAddress(const void * address, size_t size);

#if POINCARE_TREE_LOG
  void flatLog(std::ostream & stream);
  void treeLog(std::ostream & stream);
  __attribute__((__used__)) void log() { treeLog(std::cout); }
#endif
  int numberOfNodes() const;

private:
  constexpr static int BufferSize = 16384;
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);
  static TreePool * SharedStaticPool;

  // TreeNode
  void discardTreeNode(TreeNode * node);
  void registerNode(TreeNode * node);
  void unregisterNode(TreeNode * node) {
    freeIdentifier(node->identifier());
  }
  void updateNodeForIdentifierFromNode(TreeNode * node);
  void renameNode(TreeNode * node, bool unregisterPreviousIdentifier = true) {
    node->rename(generateIdentifier(), unregisterPreviousIdentifier);
  }

  // Iterators
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(constBuffer())); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  class Nodes final {
  public:
    Nodes(TreeNode * node) : m_node(node) {}
    class Iterator : public TreeNode::Iterator<TreeNode> {
    public:
      using TreeNode::Iterator<TreeNode>::Iterator;
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
  Nodes allNodes() { return Nodes(first()); }

  class RootNodes final {
  public:
    RootNodes(TreeNode * node) : m_node(node) {}
    class Iterator : public TreeNode::Iterator<TreeNode> {
    public:
      using TreeNode::Iterator<TreeNode>::Iterator;
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
  RootNodes roots() { return RootNodes(first()); }

  // Pool memory
  void dealloc(TreeNode * ptr, size_t size);
  void moveNodes(TreeNode * destination, TreeNode * source, size_t moveLength);

  // Identifiers
  int generateIdentifier() { return m_identifiers.pop(); }
  void freeIdentifier(int identifier);

  class IdentifierStack final {
  public:
    IdentifierStack() : m_currentIndex(0) {
      for (int i = 0; i < MaxNumberOfNodes; i++) {
        push(i);
      }
    }
    void push(int i) {
      assert(m_currentIndex >= 0 && m_currentIndex < MaxNumberOfNodes);
      m_availableIdentifiers[m_currentIndex++] = i;
    }
    int pop() {
      if (m_currentIndex == 0) {
        assert(false);
        return -1;
      }
      assert(m_currentIndex > 0 && m_currentIndex <= MaxNumberOfNodes);
      return m_availableIdentifiers[--m_currentIndex];
    }
  private:
    int m_currentIndex;
    int m_availableIdentifiers[MaxNumberOfNodes];
  };

  void freePoolFromNode(TreeNode * firstNodeToDiscard);

  char * buffer() { return reinterpret_cast<char *>(m_alignedBuffer); }
  const char * constBuffer() const { return reinterpret_cast<const char *>(m_alignedBuffer); }
  AlignedNodeBuffer m_alignedBuffer[BufferSize/ByteAlignment];
  char * m_cursor;
  IdentifierStack m_identifiers;
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
};

}

#endif

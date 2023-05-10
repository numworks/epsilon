#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include "tree_node.h"
#include <poincare/ghost_node.h>
#include <stddef.h>
#include <string.h>
#include <new>
#if POINCARE_TREE_LOG
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
  TreeNode * node(uint16_t identifier) const {
    assert(TreeNode::IsValidIdentifier(identifier) && identifier < MaxNumberOfNodes);
    if (m_nodeForIdentifierOffset[identifier] != UINT16_MAX) {
      return const_cast<TreeNode *>(reinterpret_cast<const TreeNode *>(m_alignedBuffer + m_nodeForIdentifierOffset[identifier]));
    }
    return nullptr;
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
  constexpr static int k_maxNodeOffset = BufferSize/ByteAlignment;

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
  uint16_t generateIdentifier() { return m_identifiers.pop(); }
  void freeIdentifier(uint16_t identifier);

  class IdentifierStack final {
  public:
    IdentifierStack() : m_currentIndex(0) {
      for (uint16_t i = 0; i < MaxNumberOfNodes; i++) {
        push(i);
      }
    }
    void push(uint16_t i) {
      assert(TreeNode::IsValidIdentifier(m_currentIndex) && m_currentIndex < MaxNumberOfNodes);
      m_availableIdentifiers[m_currentIndex++] = i;
    }
    uint16_t pop() {
      if (m_currentIndex == 0) {
        assert(false);
        return 0;
      }
      assert(m_currentIndex > 0 && m_currentIndex <= MaxNumberOfNodes);
      return m_availableIdentifiers[--m_currentIndex];
    }
  private:
    uint16_t m_currentIndex;
    uint16_t m_availableIdentifiers[MaxNumberOfNodes];
    static_assert(MaxNumberOfNodes < INT16_MAX && sizeof(m_availableIdentifiers[0]) == sizeof(uint16_t), "Tree node identifiers do not have the right data size.");
  };

  void freePoolFromNode(TreeNode * firstNodeToDiscard);

  char * buffer() { return reinterpret_cast<char *>(m_alignedBuffer); }
  const char * constBuffer() const { return reinterpret_cast<const char *>(m_alignedBuffer); }
  AlignedNodeBuffer m_alignedBuffer[BufferSize/ByteAlignment];
  char * m_cursor;
  IdentifierStack m_identifiers;
  uint16_t m_nodeForIdentifierOffset[MaxNumberOfNodes];
  static_assert(k_maxNodeOffset < UINT16_MAX && sizeof(m_nodeForIdentifierOffset[0]) == sizeof(uint16_t),
        "The tree pool node offsets in m_nodeForIdentifierOffset cannot be written with the chosen data size (uint16_t)");
};

}

#endif

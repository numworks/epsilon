#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include "tree_node.h"
#include <poincare/ghost_node.h>
#include <stddef.h>
#include <string.h>
#include <new>
#include <setjmp.h>
#if POINCARE_TREE_LOG
#include <ostream>
#include <iostream>
#endif

#define POINCARE_ALLOW_STATIC_NODES 0

namespace Poincare {

class TreeByReference;

class TreePool {
  friend class TreeNode;
  friend class TreeByReference;
public:
  static constexpr int NoNodeIdentifier = -1;

  static TreePool * sharedPool() { assert(SharedStaticPool != nullptr); return SharedStaticPool; }
  static void RegisterPool(TreePool * pool) {  assert(SharedStaticPool == nullptr); SharedStaticPool = pool; }

  TreePool() :
    m_cursor(m_buffer),
    m_currentJumpEnvironment(nullptr),
    m_endOfPoolBeforeJump(nullptr)
  {}
  void setJumpEnvironment(jmp_buf * env);
  jmp_buf * jumpEnvironment() { return m_currentJumpEnvironment; }
  void resetJumpEnvironment();

  // Node
  TreeNode * node(int identifier) const {
#if POINCARE_ALLOW_STATIC_NODES
    if (identifier <= TreeNode::FirstStaticNodeIdentifier) {
      int index = indexOfStaticNode(identifier);
      assert(index >= 0 && index < MaxNumberOfStaticNodes);
      return m_staticNodes[index];
    }
#endif
    assert(identifier >= 0 && identifier <= MaxNumberOfNodes);
    return m_nodeForIdentifier[identifier];
  }

  template <typename T>
  T * createTreeNode(size_t size = sizeof(T)) {
    int nodeIdentifier = generateIdentifier();
    void * ptr = alloc(size);
    T * node = new(ptr) T();

    // Ensure the pool is syntactially correct by creating ghost children if needed.
    // It's needed for children that have a fixed, non-zero number of children.
    for (int i = 0; i < node->numberOfChildren(); i++) {
      TreeNode * ghost = createTreeNode<GhostNode>();
      ghost->retain();
      move(node->next(), ghost, 0);
    }
    node->rename(nodeIdentifier, false);
    return node;
  }

  void move(TreeNode * destination, TreeNode * source, int realNumberOfSourceChildren);
  void moveChildren(TreeNode * destination, TreeNode * sourceParent);
  void removeChildren(TreeNode * node, int nodeNumberOfChildren);
  void removeChildrenAndDestroy(TreeNode * nodeToDestroy, int nodeNumberOfChildren);

  TreeNode * deepCopy(TreeNode * node) {
    size_t size = node->deepSize(-1);
    void * ptr = alloc(size);
    memcpy(ptr, static_cast<void *>(node), size);
    TreeNode * copy = reinterpret_cast<TreeNode *>(ptr);
    renameNode(copy, false);
    for (TreeNode * child : copy->depthFirstChildren()) {
      renameNode(child, false);
      child->retain();
    }
    return copy;
  }

#if POINCARE_ALLOW_STATIC_NODES
  void registerStaticNodeIfRequired(TreeNode * node);
  void registerStaticNode(TreeNode * node);
#endif

#if POINCARE_TREE_LOG
  void flatLog(std::ostream & stream);
  void treeLog(std::ostream & stream);
  void log() { treeLog(std::cout); }
#endif

  int numberOfNodes() const {
    int count = 0;
    TreeNode * firstNode = first();
    TreeNode * lastNode = last();
    while (firstNode != lastNode) {
      count++;
      firstNode = firstNode->next();
    }
    return count;
  }

private:
  constexpr static int BufferSize = 32768;
  constexpr static int MaxNumberOfNodes = BufferSize/sizeof(TreeNode);
#if POINCARE_ALLOW_STATIC_NODES
  constexpr static int MaxNumberOfStaticNodes = 200; // TODO: count how may are needed
#endif
  static TreePool * SharedStaticPool;

  // TreeNode
  void discardTreeNode(TreeNode * node) {
    int nodeIdentifier = node->identifier();
    size_t size = node->size();
    node->~TreeNode();
    dealloc(node, size);
    freeIdentifier(nodeIdentifier);
  }

  void registerNode(TreeNode * node) {
    int nodeID = node->identifier();
    if (nodeID >= 0 && nodeID < MaxNumberOfNodes) {
      m_nodeForIdentifier[nodeID] = node;
    }
  }

  void unregisterNode(TreeNode * node) {
    freeIdentifier(node->identifier());
  }

  void updateNodeForIdentifierFromNode(TreeNode * node);

  void renameNode(TreeNode * node, bool unregisterPreviousIdentifier = true) {
    node->rename(generateIdentifier(), unregisterPreviousIdentifier);
  }

#if POINCARE_ALLOW_STATIC_NODES
  int identifierOfStaticNodeAtIndex(int index) const { return TreeNode::FirstStaticNodeIdentifier-index;} // We do not want positive indexes that are reserved for pool nodes, and -1 is reserved for node initialisation.
  int indexOfStaticNode(int id) const { return -(id - TreeNode::FirstStaticNodeIdentifier);}
#endif

  // Iterators
  TreeNode * first() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer)); }
  TreeNode * last() const { return reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor)); }

  class Nodes {
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

  class RootNodes {
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
  void * alloc(size_t size);
  void dealloc(TreeNode * ptr, size_t size);
  bool insert(char * destination, char * source, size_t length);
  void moveNodes(TreeNode * destination, TreeNode * source, size_t moveLength);

  // Identifiers
  int generateIdentifier() {
    return m_identifiers.pop();
  }

  void freeIdentifier(int identifier) {
    if (identifier >= 0 && identifier < MaxNumberOfNodes) {
      m_nodeForIdentifier[identifier] = nullptr;
      m_identifiers.push(identifier);
    }
  }

  // Debug
  void logNodeForIdentifierArray();

  char * m_cursor;
  char m_buffer[BufferSize];
  class IdentifierStack {
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
  IdentifierStack m_identifiers;
  TreeNode * m_nodeForIdentifier[MaxNumberOfNodes];
#if POINCARE_ALLOW_STATIC_NODES
  TreeNode * m_staticNodes[MaxNumberOfStaticNodes];
#endif
  jmp_buf * m_currentJumpEnvironment; //TODO make static?
  TreeNode * m_endOfPoolBeforeJump;
};

}

#endif

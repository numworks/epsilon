#ifndef POINCARE_MEMORY_NODE_H
#define POINCARE_MEMORY_NODE_H

#include <string.h>

#include <utility>

#include "type_block.h"

#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

/* A block is a byte-long object containing either a type or some value.
 * Several blocks can form a node, like:
 * [INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
 * [ADDITION][LENGTH][ADDITION]
 * A node can also be composed of a single block:
 * [COSINE]
 */

class EditionReference;

class Node {
 public:
  // Prevent using Nodes objects directly
  Node() = delete;
  void operator=(Node&& other) = delete;

  static const Node* FromBlocks(const Block* blocks) {
    return reinterpret_cast<const Node*>(blocks);
  }

  static Node* FromBlocks(Block* blocks) {
    return reinterpret_cast<Node*>(blocks);
  }

  bool treeIsIdenticalTo(const Node* other) const {
    return memcmp(this, other, treeSize()) == 0;
  }

  bool isIdenticalTo(const Node* other) const {
    return memcmp(this, other, nodeSize()) == 0;
  }

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const { log(std::cout); }
  __attribute__((__used__)) void logDiffWith(const Node* n) const {
    log(std::cout, true, false, 0, n);
  }
  void log(std::ostream& stream, bool recursive = true, bool verbose = true,
           int indentation = 0, const Node* comparison = nullptr) const;
  void logName(std::ostream& stream) const;
  void logAttributes(std::ostream& stream) const;
  __attribute__((__used__)) void logBlocks() const { logBlocks(std::cout); }
  void logBlocks(std::ostream& stream, bool recursive = true,
                 int indentation = 0) const;
#endif

  constexpr const TypeBlock* block() const { return m_block; }
  constexpr TypeBlock* block() { return m_block; }
  void copyTreeTo(void* address) const;

  // Node Navigation
  const Node* nextNode() const;
  Node* nextNode() { return Utils::DeconstifyPtr(&Node::nextNode, this); };
  /* TODO : If costly, previousNode could be optimized by sprinkling TreeBorders
   *        blocks between every tree of the cache. PreviousNode and nextNode
   *        would only have to check for TreeBorders. */
  const Node* previousNode() const;
  Node* previousNode() {
    return Utils::DeconstifyPtr(&Node::previousNode, this);
  }
  const Node* nextTree() const {
    const Node* result = this;
    int nbOfChildrenToScan = result->numberOfChildren();
    while (nbOfChildrenToScan > 0) {
      result = result->nextNode();
      nbOfChildrenToScan += result->numberOfChildren() - 1;
    }
    return result->nextNode();
  }
  Node* nextTree() { return Utils::DeconstifyPtr(&Node::nextTree, this); };
  const Node* previousTree() const;
  Node* previousTree() {
    return Utils::DeconstifyPtr(&Node::previousTree, this);
  }

  // Sizes
  size_t treeSize() const { return nextTree()->block() - block(); }

  // Node Hierarchy
  /* TODO : parent, previousBlock and similar methods navigating backward could
   *         be forbidden and deleted, optimizing node size and navigation. */
  const Node* parent() const;
  Node* parent() { return Utils::DeconstifyPtr(&Node::parent, this); }
  const Node* commonAncestor(const Node* child1, const Node* child2) const;
  const Node* parentOfDescendant(const Node* descendant, int* position) const;
  int numberOfDescendants(bool includeSelf) const;
  const Node* childAtIndex(int index) const;
  Node* childAtIndex(int index) {
    return Utils::DeconstifyPtr(&Node::childAtIndex, this, index);
  }
  int indexOfChild(const Node* child) const;
  bool hasChild(const Node* child) const;
  bool hasAncestor(const Node* node, bool includeSelf) const;

  constexpr BlockType type() const { return m_block->type(); }
  constexpr size_t nodeSize() const { return m_block->nodeSize(true); }
  constexpr int numberOfChildren() const {
    return m_block->numberOfChildren(true);
  }
  constexpr bool isNAry() const { return m_block->isNAry(); }

  EditionReference clone() const;

  // Edition
  void moveNodeAfterNode(Node* n) { moveAt(n, false, false); }
  void moveTreeAfterNode(Node* n) { moveAt(n, false, true); }
  void moveNodeBeforeNode(Node* n) { moveAt(n, true, false); }
  void moveTreeBeforeNode(Node* n) { moveAt(n, true, true); }
  void cloneNodeAfterNode(const Node* n) { cloneAt(n, false, false); }
  void cloneTreeAfterNode(const Node* n) { cloneAt(n, false, true); }
  void cloneNodeBeforeNode(const Node* n) { cloneAt(n, true, false); }
  void cloneTreeBeforeNode(const Node* n) { cloneAt(n, true, true); }
  Node* moveNodeOverNode(Node* n) { return moveOver(n, false, false); }
  Node* moveTreeOverNode(Node* n) { return moveOver(n, false, true); }
  Node* moveNodeOverTree(Node* n) { return moveOver(n, true, false); }
  Node* moveTreeOverTree(Node* n) { return moveOver(n, true, true); }
  Node* cloneNodeOverNode(const Node* n) { return cloneOver(n, false, false); }
  Node* cloneTreeOverNode(const Node* n) { return cloneOver(n, false, true); }
  Node* cloneNodeOverTree(const Node* n) { return cloneOver(n, true, false); }
  Node* cloneTreeOverTree(const Node* n) { return cloneOver(n, true, true); }

  // Move the node/tree to the end of the pool
  Node* detachNode() { return detach(false); };
  Node* detachTree() { return detach(true); };
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }

  // Iterators
  class AbstractIterator {
   public:
    AbstractIterator(const Node* node) : m_node(node) {}
    const Node* operator*() { return m_node; }
    bool operator!=(const AbstractIterator& it) const {
      return m_node != it.m_node;
    }

   protected:
    const Node* m_node;
  };

  template <class Iterator>
  class ConstRange final {
   public:
    ConstRange(const Node* begin, const Node* end)
        : m_begin(begin), m_end(end) {}
    Iterator begin() const { return m_begin; }
    Iterator end() const { return m_end; }

   private:
    const Node* m_begin;
    const Node* m_end;
  };

  class ConstTreeIterator : public AbstractIterator {
   public:
    using AbstractIterator::AbstractIterator;
    ConstTreeIterator& operator++() {
      m_node = m_node->nextTree();
      return *this;
    }
  };

  class ConstNodeIterator : public AbstractIterator {
   public:
    using AbstractIterator::AbstractIterator;
    ConstNodeIterator& operator++() {
      m_node = m_node->nextNode();
      return *this;
    }
  };

  using ConstNodeRange = ConstRange<ConstNodeIterator>;
  using ConstTreeRange = ConstRange<ConstTreeIterator>;

  ConstTreeRange children() const {
    return ConstTreeRange(nextNode(), nextTree());
  }

 private:
  bool canNavigateNext() const;
  bool canNavigatePrevious() const;
  const Node* previousRelative(bool parent) const;

  void cloneAt(const Node* nodeToClone, bool before, bool newIsTree);
  void moveAt(Node* nodeToMove, bool before, bool newIsTree);
  Node* cloneOver(const Node* n, bool oldIsTree, bool newIsTree);
  Node* moveOver(Node* n, bool oldIsTree, bool newIsTree);
  Node* detach(bool isTree);
  void remove(bool isTree);

  // Should be last - and most likely only - member
  TypeBlock m_block[0];
};

}  // namespace PoincareJ

#endif

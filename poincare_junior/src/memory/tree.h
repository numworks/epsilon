#ifndef POINCARE_MEMORY_TREE_H
#define POINCARE_MEMORY_TREE_H

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

class Tree {
 public:
  // Prevent using Nodes objects directly
  Tree() = delete;
  void operator=(Tree&& other) = delete;

  static const Tree* FromBlocks(const Block* blocks) {
    return reinterpret_cast<const Tree*>(blocks);
  }

  static Tree* FromBlocks(Block* blocks) {
    return reinterpret_cast<Tree*>(blocks);
  }

  bool treeIsIdenticalTo(const Tree* other) const {
    return memcmp(this, other, treeSize()) == 0;
  }

  bool nodeIsIdenticalTo(const Tree* other) const {
    return memcmp(this, other, nodeSize()) == 0;
  }

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const { log(std::cout); }
  __attribute__((__used__)) void logDiffWith(const Tree* n) const {
    log(std::cout, true, false, 0, n);
  }
  void log(std::ostream& stream, bool recursive = true, bool verbose = true,
           int indentation = 0, const Tree* comparison = nullptr) const;
  void logName(std::ostream& stream) const;
  void logAttributes(std::ostream& stream) const;
  __attribute__((__used__)) void logBlocks() const { logBlocks(std::cout); }
  void logBlocks(std::ostream& stream, bool recursive = true,
                 int indentation = 0) const;
#endif

  constexpr const TypeBlock* block() const { return m_block; }
  constexpr TypeBlock* block() { return m_block; }
  void copyTreeTo(void* address) const;

  // Tree Navigation
  const Tree* nextNode() const;
  Tree* nextNode() { return Utils::DeconstifyPtr(&Tree::nextNode, this); };
  const Tree* nextTree() const {
    const Tree* result = this;
    int nbOfChildrenToScan = result->numberOfChildren();
    while (nbOfChildrenToScan > 0) {
      result = result->nextNode();
      nbOfChildrenToScan += result->numberOfChildren() - 1;
    }
    return result->nextNode();
  }
  Tree* nextTree() { return Utils::DeconstifyPtr(&Tree::nextTree, this); };

  // Sizes
  size_t treeSize() const { return nextTree()->block() - block(); }

  // Tree Hierarchy
  const Tree* commonAncestor(const Tree* child1, const Tree* child2) const;
  const Tree* parentOfDescendant(const Tree* descendant, int* position) const;
  // Make position optional
  const Tree* parentOfDescendant(const Tree* descendant) const {
    int dummyPosition;
    return parentOfDescendant(descendant, &dummyPosition);
  }
  int numberOfDescendants(bool includeSelf) const;
  const Tree* childAtIndex(int index) const;
  Tree* childAtIndex(int index) {
    return Utils::DeconstifyPtr(&Tree::childAtIndex, this, index);
  }
  int indexOfChild(const Tree* child) const;
  bool hasChild(const Tree* child) const;
  bool hasAncestor(const Tree* node, bool includeSelf) const;

  constexpr BlockType type() const { return m_block->type(); }
  constexpr size_t nodeSize() const { return m_block->nodeSize(); }
  constexpr int numberOfChildren() const { return m_block->numberOfChildren(); }
  constexpr bool isNAry() const { return m_block->isNAry(); }

  Tree* clone() const;

  // Tree motions
  // TODO
  /* In the following comments aaaa represents several blocks of a tree and u
   * and v are EditionReferences that could be pointing to the targeted trees.
   * Beware that the Node * used in this methods (this and n) may point to a
   * completely invalid position afterward (especially when the source was
   * before the target in the pool).
   */

  /*  u     v      u.moveBefore(v)  v  u
   *  |     |            =>         |  |
   *  aaaabbcccdd                   cccaaaabbdd
   */
  Tree* moveNodeBeforeNode(Tree* n) { return moveAt(n, true, false); }
  Tree* moveTreeBeforeNode(Tree* n) { return moveAt(n, true, true); }
  Tree* cloneNodeBeforeNode(const Tree* n) { return cloneAt(n, true, false); }
  Tree* cloneTreeBeforeNode(const Tree* n) { return cloneAt(n, true, true); }

  /*  u     v        u.moveAt(v)   u+v
   *  |     |            =>         |
   *  aaaabbcccdd                   cccaaaabbdd
   */
  void moveNodeAtNode(Tree* n) { moveAt(n, true, false, true); }
  void moveTreeAtNode(Tree* n) { moveAt(n, true, true, true); }
  void cloneNodeAtNode(const Tree* n) { cloneAt(n, true, false, true); }
  void cloneTreeAtNode(const Tree* n) { cloneAt(n, true, true, true); }

  /*  u     v                       u   v
   *  |     |      u.moveAfter(v)   |   |
   *  aaaabbcccdd        =>         aaaacccbbdd
   */
  void moveNodeAfterNode(Tree* n) { moveAt(n, false, false); }
  void moveTreeAfterNode(Tree* n) { moveAt(n, false, true); }
  void cloneNodeAfterNode(const Tree* n) { cloneAt(n, false, false); }
  void cloneTreeAfterNode(const Tree* n) { cloneAt(n, false, true); }

  /*  u     v                       v
   *  |     |       u.moveOver(v)   |
   *  aaaabbcccdd        =>         cccbbdd
   */
  Tree* moveNodeOverNode(Tree* n) { return moveOver(n, false, false); }
  Tree* moveTreeOverNode(Tree* n) { return moveOver(n, false, true); }
  Tree* moveNodeOverTree(Tree* n) { return moveOver(n, true, false); }
  Tree* moveTreeOverTree(Tree* n) { return moveOver(n, true, true); }
  Tree* cloneNodeOverNode(const Tree* n) { return cloneOver(n, false, false); }
  Tree* cloneTreeOverNode(const Tree* n) { return cloneOver(n, false, true); }
  Tree* cloneNodeOverTree(const Tree* n) { return cloneOver(n, true, false); }
  Tree* cloneTreeOverTree(const Tree* n) { return cloneOver(n, true, true); }

  /*    u   v                      v
   *    |   |       u.remove()     |
   *  aabbbbcccdd       =>       aacccdd
   */
  void removeNode() { remove(false); }
  void removeTree() { remove(true); }

  /*    u   v                      v       u
   *    |   |       u.detach()     |       |
   *  aabbbbcccdd       =>       aacccdd...bbbb
   */
  Tree* detachNode() { return detach(false); };
  Tree* detachTree() { return detach(true); };

  // Iterators
  class AbstractIterator {
   public:
    AbstractIterator(const Tree* node) : m_node(node) {}
    const Tree* operator*() { return m_node; }
    bool operator!=(const AbstractIterator& it) const {
      return m_node != it.m_node;
    }

   protected:
    const Tree* m_node;
  };

  template <class Iterator>
  class ConstRange final {
   public:
    ConstRange(const Tree* begin, const Tree* end)
        : m_begin(begin), m_end(end) {}
    Iterator begin() const { return m_begin; }
    Iterator end() const { return m_end; }

   private:
    const Tree* m_begin;
    const Tree* m_end;
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
  Tree* cloneAt(const Tree* nodeToClone, bool before, bool newIsTree,
                bool at = false);
  Tree* moveAt(Tree* nodeToMove, bool before, bool newIsTree, bool at = false);
  Tree* cloneOver(const Tree* n, bool oldIsTree, bool newIsTree);
  Tree* moveOver(Tree* n, bool oldIsTree, bool newIsTree);
  Tree* detach(bool isTree);
  void remove(bool isTree);

  // Should be last - and most likely only - member
  TypeBlock m_block[0];
};

}  // namespace PoincareJ

#endif

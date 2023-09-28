#ifndef POINCARE_MEMORY_TREE_H
#define POINCARE_MEMORY_TREE_H

#include <string.h>

#include <utility>

#include "type_block.h"
#include "value_block.h"

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
#if !PLATFORM_DEVICE
  static uint32_t nextNodeCount;
  static uint32_t nextNodeInPoolCount;
#endif
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

  // Get the i-th value block (after the type block)
  constexpr uint8_t nodeValue(uint8_t index) const {
    assert(index + 1 < nodeSize());
    return static_cast<uint8_t>(m_block[index + 1]);
  }
  void setNodeValue(int index, uint8_t value) {
    assert(index + 1 < nodeSize());
    m_block[index + 1] = Block(value);
  }
  ValueBlock* nodeValueBlock(int index) {
    return static_cast<ValueBlock*>(&m_block[index + 1]);
  }

  constexpr const Block* block() const { return m_block; }
  constexpr Block* block() { return m_block; }
  void copyTreeTo(void* address) const;

  // Tree Navigation
  const Tree* nextNode() const;
  Tree* nextNode() { return Utils::DeconstifyPtr(&Tree::nextNode, this); };
  const Tree* firstChild() const { return nextNode(); }
  Tree* firstChild() { return nextNode(); }
  const Tree* nextTree() const {
    const Tree* result = this;
    int nbOfChildrenToScan = 1;
    while (nbOfChildrenToScan > 0) {
      nbOfChildrenToScan += result->numberOfChildren() - 1;
      result = result->nextNode();
    }
    return result;
  }
  Tree* nextTree() { return Utils::DeconstifyPtr(&Tree::nextTree, this); };

  // Sizes
  size_t treeSize() const { return nextTree()->block() - block(); }

  // Tree Hierarchy
  const Tree* commonAncestor(const Tree* child1, const Tree* child2) const;
  Tree* commonAncestor(const Tree* child1, const Tree* child2) {
    // Children may be const but they belong to root which is non-const anyway
    return Utils::DeconstifyPtr(&Tree::commonAncestor, this, child1, child2);
  }
  const Tree* parentOfDescendant(const Tree* descendant, int* position) const;
  Tree* parentOfDescendant(Tree* descendant, int* position) const {
    return const_cast<Tree*>(
        parentOfDescendant(const_cast<const Tree*>(descendant), position));
  }
  // Make position optional
  const Tree* parentOfDescendant(const Tree* descendant) const {
    int dummyPosition;
    return parentOfDescendant(descendant, &dummyPosition);
  }
  Tree* parentOfDescendant(const Tree* descendant) {
    int dummyPosition;
    return Utils::DeconstifyPtr(&Tree::parentOfDescendant, this, descendant,
                                &dummyPosition);
  }
  int numberOfDescendants(bool includeSelf) const;
  const Tree* childAtIndex(int index) const;
  Tree* childAtIndex(int index) {
    return Utils::DeconstifyPtr(&Tree::childAtIndex, this, index);
  }
  int indexOfChild(const Tree* child) const;
  bool hasChild(const Tree* child) const;
  bool hasAncestor(const Tree* node, bool includeSelf) const;

  constexpr TypeBlock type() const { return *typeBlock(); }
  constexpr LayoutType layoutType() const {
    assert(type().isLayout());
    return static_cast<LayoutType>(static_cast<BlockType>(type().type()));
  }
  constexpr size_t nodeSize() const { return typeBlock()->nodeSize(); }
  constexpr int numberOfChildren() const {
    return typeBlock()->numberOfChildren();
  }
  constexpr bool isNAry() const { return typeBlock()->isNAry(); }

  Tree* clone() const;
  Tree* cloneNode() const;

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

 private:
  // Iterators
  template <typename T>
  class AbstractIterator {
   public:
    using Type = T;
    AbstractIterator(Type node, int remaining)
        : m_node(node), m_remaining(remaining) {}
    Type operator*() { return m_node; }
    bool operator!=(const AbstractIterator& it) const {
      // All Iterators with nothing remaining compare equal
      return m_remaining > 0 && m_node != it.m_node;
    }

   protected:
    Type m_node;
    int m_remaining;
  };

  template <class Iterator>
  class ElementList final {
    using Type = typename Iterator::Type;

   public:
    ElementList(Type begin, int remaining)
        : m_begin(begin), m_remaining(remaining) {}
    Iterator begin() const { return Iterator(m_begin, m_remaining); }
    Iterator end() const { return Iterator(nullptr, 0); }

   private:
    Type m_begin;
    int m_remaining;
  };

  template <typename T>
  class ChildrenIterator : public AbstractIterator<T> {
    using Parent = AbstractIterator<T>;

   public:
    using Parent::AbstractIterator;
    ChildrenIterator<T>& operator++() {
      Parent::m_remaining--;
      Parent::m_node = Parent::m_node->nextTree();
      return *this;
    }
  };

  template <typename T>
  class DescendantsIterator : public AbstractIterator<T> {
    using Parent = AbstractIterator<T>;

   public:
    using Parent::AbstractIterator;
    DescendantsIterator<T>& operator++() {
      Parent::m_remaining += Parent::m_node->numberOfChildren() - 1;
      Parent::m_node = Parent::m_node->nextNode();
      return *this;
    }
  };

 public:
  using ConstTrees = ElementList<ChildrenIterator<const Tree*>>;
  using Trees = ElementList<ChildrenIterator<Tree*>>;
  using ConstNodes = ElementList<DescendantsIterator<const Tree*>>;
  using Nodes = ElementList<DescendantsIterator<Tree*>>;

  ConstNodes selfAndDescendants() const { return {this, 1}; }
  // Do not alter number of children while iterating
  Nodes selfAndDescendants() { return {this, 1}; }

  ConstTrees children() const { return {nextNode(), numberOfChildren()}; }
  ConstNodes descendants() const { return {nextNode(), numberOfChildren()}; }
  // Do not alter number of children while iterating
  Trees children() { return {nextNode(), numberOfChildren()}; }
  // Do not alter number of children while iterating
  Nodes descendants() { return {nextNode(), numberOfChildren()}; }

 private:
  Tree* cloneAt(const Tree* nodeToClone, bool before, bool newIsTree,
                bool at = false);
  Tree* moveAt(Tree* nodeToMove, bool before, bool newIsTree, bool at = false);
  Tree* cloneOver(const Tree* n, bool oldIsTree, bool newIsTree);
  Tree* moveOver(Tree* n, bool oldIsTree, bool newIsTree);
  Tree* detach(bool isTree);
  void remove(bool isTree);

  constexpr const TypeBlock* typeBlock() const {
    return static_cast<const TypeBlock*>(static_cast<const Block*>(m_block));
  }

  // Should be last - and most likely only - member
  Block m_block[0];
};

void SwapTrees(Tree* u, Tree* v);

/* This overload exchanges the trees pointed to by the pointers and update the
 * pointers. */
void SwapTrees(Tree** u, Tree** v);

}  // namespace PoincareJ

#endif

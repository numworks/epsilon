#ifndef POINCARE_MEMORY_TREE_H
#define POINCARE_MEMORY_TREE_H

#include <string.h>

#include <utility>

#include "type_block.h"
#include "value_block.h"

#if POINCARE_TREE_LOG
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

class Tree : public TypeBlock {
 public:
#if PCJ_METRICS
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

#if POINCARE_TREE_LOG
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
  __attribute__((__used__)) void logSerialize() const {
    logSerialize(std::cout);
  }
  void logSerialize(std::ostream& stream) const;
#endif

  // Get the i-th value block (after the type block)
  constexpr uint8_t nodeValue(uint8_t index) const {
    assert(index + 1 < static_cast<uint8_t>(nodeSize()));
    return static_cast<uint8_t>(m_valueBlocks[index]);
  }
  void setNodeValue(uint8_t index, uint8_t value) {
    assert(index + 1 < static_cast<uint8_t>(nodeSize()));
    m_valueBlocks[index] = value;
  }
  ValueBlock* nodeValueBlock(uint8_t index) { return &m_valueBlocks[index]; }
  const ValueBlock* nodeValueBlock(uint8_t index) const {
    return &m_valueBlocks[index];
  }

  constexpr const Block* block() const { return this; }
  constexpr Block* block() { return this; }
  void copyTreeTo(void* address) const;

  // Tree Navigation
  const Tree* nextNode() const;
  Tree* nextNode() {
    return const_cast<Tree*>(const_cast<const Tree*>(this)->nextNode());
  }
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
  Tree* nextTree() {
    return const_cast<Tree*>(const_cast<const Tree*>(this)->nextTree());
  }

  // Sizes
  size_t treeSize() const { return nextTree()->block() - block(); }

  // Tree Hierarchy
  const Tree* commonAncestor(const Tree* child1, const Tree* child2) const;
  Tree* commonAncestor(const Tree* child1, const Tree* child2) {
    // Children may be const but they belong to root which is non-const anyway
    return const_cast<Tree*>(
        const_cast<const Tree*>(this)->commonAncestor(child1, child2));
  }
  const Tree* parent(const Tree* root) const {
    return root->parentOfDescendant(this);
  }
  Tree* parent(Tree* root) { return root->parentOfDescendant(this); }
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
  const Tree* child(int index) const;
  Tree* child(int index) {
    return const_cast<Tree*>(const_cast<const Tree*>(this)->child(index));
  }
  int indexOfChild(const Tree* child) const;
  bool hasChild(const Tree* child) const;
  bool hasAncestor(const Tree* node, bool includeSelf) const;
  const Tree* lastChild() const { return child(numberOfChildren() - 1); }

  constexpr TypeBlock type() const { return *this; }
  constexpr LayoutType layoutType() const {
    assert(type().isLayout());
    return static_cast<LayoutType>(static_cast<Type>(type().type()));
  }

  typedef bool (*Operation)(Tree* node);
  // Apply Shallow operation in a Top Down manner.
  typedef bool (*ShallowOperation)(Tree* node, void* context);
  static bool ApplyShallowInDepth(Tree* node, ShallowOperation shallowOperation,
                                  void* context = nullptr, bool check = true);

  bool deepReplaceWith(const Tree* target, const Tree* replacement);

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

  /*  u     v                       v    u
   *  |     |       u.swapWith(v)   |    |
   *  aaaabbcccdd        =>         cccbbaaaadd
   */
  void swapWithTree(Tree* v);

 protected:
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

  template <typename T>
  class RootToNodeIterator {
   public:
    using Type = T;
    RootToNodeIterator(Type root, Type node) : m_root(root), m_node(node) {
      assert(root <= node && node < root->nextTree());
    }
    Type operator*() { return m_root; }
    bool operator!=(const RootToNodeIterator& it) const {
      // Assume iterators with different targets are never compared
      return m_root != it.m_root;
    }
    RootToNodeIterator<T>& operator++() {
      T child = m_root->child(0);
      while (true) {
        T nextChild = child->nextTree();
        if (nextChild > m_node) {
          m_root = child;
          return *this;
        }
        child = nextChild;
      }
    }

   private:
    T m_root;
    T m_node;
  };

  template <class Iterator>
  class TargetList final {
    using Type = typename Iterator::Type;

   public:
    TargetList(Type begin, Type end) : m_begin(begin), m_end(end) {}
    Iterator begin() const { return Iterator(m_begin, m_end); }
    Iterator end() const { return Iterator(m_end, m_end); }

   private:
    Type m_begin;
    Type m_end;
  };

 public:
  using ConstTrees = ElementList<ChildrenIterator<const Tree*>>;
  using Trees = ElementList<ChildrenIterator<Tree*>>;
  using ConstNodes = ElementList<DescendantsIterator<const Tree*>>;
  using Nodes = ElementList<DescendantsIterator<Tree*>>;
  using ConstAncestors = TargetList<RootToNodeIterator<const Tree*>>;

  ConstNodes selfAndDescendants() const { return {this, 1}; }
  // Do not alter number of children while iterating
  Nodes selfAndDescendants() { return {this, 1}; }

  ConstTrees children() const { return {nextNode(), numberOfChildren()}; }
  ConstNodes descendants() const { return {nextNode(), numberOfChildren()}; }
  // Do not alter number of children while iterating
  Trees children() { return {nextNode(), numberOfChildren()}; }
  // Do not alter number of children while iterating
  Nodes descendants() { return {nextNode(), numberOfChildren()}; }

  ConstAncestors ancestors(const Tree* root) const { return {root, this}; }

  using Predicate = bool (*)(const Tree*);
  bool hasDescendantSatisfying(Predicate predicate) const;
  bool hasChildSatisfying(Predicate predicate) const;

 private:
  bool replaceWith(const Tree* target, const Tree* replacement);
  Tree* cloneAt(const Tree* nodeToClone, bool before, bool newIsTree,
                bool at = false);
  Tree* moveAt(Tree* nodeToMove, bool before, bool newIsTree, bool at = false);
  Tree* cloneOver(const Tree* n, bool oldIsTree, bool newIsTree);
  Tree* moveOver(Tree* n, bool oldIsTree, bool newIsTree);
  Tree* detach(bool isTree);
  void remove(bool isTree);

  // Forbid tree == Type::Addition
  using TypeBlock::operator==, TypeBlock::operator!=,
      TypeBlock::operator PoincareJ::Type;

  // A tree is made of 1 TypeBlock (inherited) and nodeSize()-1 ValueBlocks
  // Should be last - and most likely only - member
  ValueBlock m_valueBlocks[0];
};

/* Even if Trees can be larger than one block, it's pratical if their C++ sizes
 * match to be able to write Tree * ptr = tree + treeSize() and so on. */
static_assert(sizeof(Tree) == sizeof(Block));

/* This overload exchanges the trees pointed to by the pointers and update the
 * pointers. */
void SwapTreesPointers(Tree** u, Tree** v);

}  // namespace PoincareJ

#endif

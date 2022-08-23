#ifndef POINCARE_TREE_BLOCK_H
#define POINCARE_TREE_BLOCK_H

#include "handle.h"

namespace Poincare {

/* A block is a byte-long object containing either a type or some value.
 * Several blocks can form a node, like:
 * [INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
 * [ADDITION][LENGTH][ADDITION]
 * A node can also be composed of a single block:
 * [COSINE]
 */

enum class BlockType : uint8_t {
#if GHOST_REQUIRED
  Ghost,
#endif
  Integer,
  IntegerShort,
  Float,
  Addition,
  Multiplication,
  Subtraction,
  Division,
  Power
};

class TreeSandbox;
class TypeTreeBlock;

struct IndexedTypeTreeBlock {
  TypeTreeBlock * m_block;
  int m_index;
};

typedef void (TypeTreeBlock::*InPlaceTreeFunction)();

class TreeBlock {
friend class TreePool;
public:
  constexpr TreeBlock(uint8_t content = 0) : m_content(content) {}
  bool operator!=(const TreeBlock& b) { return b.m_content != m_content; }

  // Block Navigation
  TreeBlock * nextBlock() { return this + 1; }
  const TreeBlock * nextBlock() const { return this + 1; }
  TreeBlock * nextNthBlock(int i) { return this + i; }
  const TreeBlock * nextNthBlock(int i) const { return this + i; }
  TreeBlock * previousBlock() { return this - 1; }
  const TreeBlock * previousBlock() const { return this - 1; }
  TreeBlock * previousNthBlock(int i) { return this - i; }
  const TreeBlock * previousNthBlock(int i) const { return this - i; }

protected:
  uint8_t m_content;
};

// This helper wraps a motion on const blocks into a motion on non-const blocks.
// It should be completely eliminated by the compiler after the type checking.
typedef const TypeTreeBlock * (TypeTreeBlock::*ConstMotion)() const;
inline TypeTreeBlock * motion(TypeTreeBlock *object, ConstMotion constMotion) {
  return const_cast<TypeTreeBlock*>((const_cast<const TypeTreeBlock*>(object)->*constMotion)());
}

// TODO: merge with previous one ?
template<typename T>
inline TypeTreeBlock * motion(TypeTreeBlock *object,  const TypeTreeBlock * (TypeTreeBlock::*constMotion)(T) const, T arg) {
  return const_cast<TypeTreeBlock*>((const_cast<const TypeTreeBlock*>(object)->*constMotion)(arg));
}

class TypeTreeBlock : public TreeBlock {
public:
  constexpr TypeTreeBlock(BlockType content = BlockType::Integer) : TreeBlock(static_cast<uint8_t>(content)) {}

#if POINCARE_TREE_LOG
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true) const;
#endif

  void copyTo(TreeBlock * address) const;

  // Block Navigation
  const TypeTreeBlock * nextNode() const;
  TypeTreeBlock * nextNode() { return motion(this, &TypeTreeBlock::nextNode); };
  const TypeTreeBlock * previousNode(const TreeBlock * firstBlock) const;
  TypeTreeBlock * previousNode(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::previousNode, firstBlock);}
  const TypeTreeBlock * nextSibling() const;
  TypeTreeBlock * nextSibling() { return motion(this, &TypeTreeBlock::nextSibling); };
  const TypeTreeBlock * previousSibling(const TreeBlock * firstBlock) const;
  TypeTreeBlock * previousSibling(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::previousSibling, firstBlock);}

  // Sizes
  size_t treeSize() const { return nextSibling() - this; }

  // Node Hierarchy
  const TypeTreeBlock * parent(const TreeBlock * firstBlock) const;
  TypeTreeBlock * parent(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::parent, firstBlock);}
  const TypeTreeBlock * root(const TreeBlock * firstBlock) const;
  TypeTreeBlock * root(const TreeBlock * firstBlock) { return motion(this, &TypeTreeBlock::root, firstBlock);}
  int numberOfDescendants(bool includeSelf) const;
  const TypeTreeBlock * childAtIndex(int index) const;
  TypeTreeBlock * childAtIndex(int index) { return motion(this, &TypeTreeBlock::childAtIndex, index);}
  int indexOfChild(const TypeTreeBlock * child) const;
  int indexInParent(const TreeBlock * firstBlock) const;
  bool hasChild(const TypeTreeBlock * child) const;
  bool hasAncestor(const TreeBlock * firstBlock, const TypeTreeBlock * node, bool includeSelf) const;
  bool hasSibling(const TreeBlock * firstBlock, const TypeTreeBlock * e) const;

  // Virtuality
#if GHOST_REQUIRED
  static constexpr Ghost k_ghost;
#endif
  static constexpr Integer k_integer;
  static constexpr Addition k_addition;
  static constexpr Multiplication k_multiplication;
  static constexpr Subtraction k_subtraction;
  static constexpr Division k_division;
  static constexpr Power k_power;
  static constexpr const Handle * k_handles[] = {
  // Order has to be the same as TypeTreeBlock
#if GHOST_REQUIRED
    &k_ghost,
#endif
    &k_integer,
    &k_integer,
    &k_integer,
    &k_addition,
    &k_multiplication,
    &k_subtraction,
    &k_division,
    &k_power
  };

  const Handle * handle() const { return k_handles[m_content]; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const { handle()->logNodeName(stream); }
  void logAttributes(std::ostream & stream) const { handle()->logAttributes(this, stream); }
#endif
  void basicReduction() { handle()->basicReduction(this); }
  size_t nodeSize(bool head = true) const { return handle()->nodeSize(this, head); } // Should it be virtual?
  int numberOfChildren() const { return handle()->numberOfChildren(this); } // Should it be virtual

  // TODO: dynamic_cast-like that can check its is a subclass with m_content
  void beautify() { static_cast<const InternalHandle*>(handle())->Beautify(this); }

  class ForwardDirect final {
  public:
    ForwardDirect(TypeTreeBlock * block) : m_block(block) {}
    class Iterator {
    public:
      Iterator(const TypeTreeBlock * block, int index) : m_indexedBlock({.m_block = const_cast<TypeTreeBlock *>(block), .m_index = index}) {}
      IndexedTypeTreeBlock operator*() { return m_indexedBlock; }
      bool operator!=(const Iterator& it) const { return (m_indexedBlock.m_index != it.m_indexedBlock.m_index); }
      Iterator & operator++() {
        m_indexedBlock.m_block = m_indexedBlock.m_block->nextSibling();
        m_indexedBlock.m_index++;
        return *this;
      }
    private:
      IndexedTypeTreeBlock m_indexedBlock;
    };
    Iterator begin() const { return Iterator(m_block->nextNode(), 0); }
    Iterator end() const { return Iterator(nullptr, m_block->numberOfChildren()); }
  private:
    TypeTreeBlock * m_block;
  };

  class BackwardsDirect final {
  public:
    BackwardsDirect(TypeTreeBlock * block) : m_memoizer(block) {}

    // TODO: explain why memoizing
    class Iterator {
    public:

      class Memoizer {
      public:
        Memoizer(TypeTreeBlock * treeBlock);
        TypeTreeBlock * childAtIndex(int i);
        size_t numberOfChildren() { return m_numberOfChildren; }
      private:
        void memoizeUntilIndex(int i);
        TypeTreeBlock * m_block;
        // Memoization of children addresses in a RingBuffer
        constexpr static size_t k_maxNumberOfMemoizedSubtrees = 16;
        TypeTreeBlock * m_children[k_maxNumberOfMemoizedSubtrees];
        size_t m_firstMemoizedSubtreeIndex; // Index used for ring buffer
        size_t m_firstSubtreeIndex;
        size_t m_numberOfChildren;
      };

      Iterator(int childIndex, Memoizer * memoizer) :
        m_childIndex(childIndex),
        m_memoizer(memoizer) {}
      TypeTreeBlock * operator*() { return m_memoizer->childAtIndex(m_childIndex); }
      bool operator!=(const Iterator& it) const { return (m_childIndex != it.m_childIndex); }

      Iterator & operator++() {
        m_childIndex--;
        return *this;
      }
    private:
      int m_childIndex;
      mutable Memoizer * m_memoizer;
    };
    Iterator begin() const { return Iterator(m_memoizer.numberOfChildren() - 1, &m_memoizer); }
    Iterator end() const { return Iterator(-1, &m_memoizer); }
  private:
    mutable Iterator::Memoizer m_memoizer;
  };

  ForwardDirect directChildren() const { return ForwardDirect(const_cast<TypeTreeBlock *>(this)); }
  BackwardsDirect backwardsDirectChildren() const { return BackwardsDirect(const_cast<TypeTreeBlock *>(this)); }

  BlockType type() const { return static_cast<BlockType>(m_content); }

  // Recursive helper
  void recursivelyApply(InPlaceTreeFunction treeFunction);

private:
  const TypeTreeBlock * previousRelative(const TreeBlock * firstBlock, bool parent) const;
#if GHOST_REQUIRED
  bool isGhost() const { return type() == BlockType::Ghost; }
#endif
};

class ValueTreeBlock : public TreeBlock {
public:
  constexpr ValueTreeBlock(uint8_t value) : TreeBlock(value) {}
  uint8_t value() const { return m_content; }
};

static_assert(sizeof(TreeBlock) == 1);
static_assert(sizeof(ValueTreeBlock) == sizeof(TreeBlock));
static_assert(sizeof(TypeTreeBlock) == sizeof(TreeBlock));

constexpr static TypeTreeBlock AdditionBlock() { return TypeTreeBlock(BlockType::Addition); }
constexpr static TypeTreeBlock MultiplicationBlock() { return TypeTreeBlock(BlockType::Multiplication); }
constexpr static TypeTreeBlock IntegerBlock() { return TypeTreeBlock(BlockType::Integer); }
constexpr static TypeTreeBlock SubtractionBlock() { return TypeTreeBlock(BlockType::Subtraction); }
constexpr static TypeTreeBlock DivisionBlock() { return TypeTreeBlock(BlockType::Division); }
constexpr static TypeTreeBlock PowerBlock() { return TypeTreeBlock(BlockType::Power); }

}

#endif

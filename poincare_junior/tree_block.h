#ifndef POINCARE_TREE_BLOCK_H
#define POINCARE_TREE_BLOCK_H

#define POINCARE_TREE_LOG 1
#if POINCARE_TREE_LOG
#include <ostream>
#endif

#include <stdint.h>
#include <stddef.h>

/* 3 options to sequentially store expressions:
 * Let's take the example 4294967298 + cos(3)
 * (keeping in mind that 4294967298 = 2^32 + 2)
 *
 * () represents nodes
 * [] represents blocks
 * Trees are derived from parsing
 *
 * - option A
 *   Nodes have variable sizes that are computed dynamically
 *   ( + 2 ) (int 1 2) (cos ) (int 3)
 * - option B
 *   Nodes are composed of several blocks, the head blocks have always their
 *   first bit set 0 and the tail blocks have their first bit set to 1 (drawn by ^)
 *   ([+][^2])([int][^1][^2])([cos])([int][^3])
 * - option C
 *   Nodes are composed of several blocks. Block types and length are indicated as head and tail.
 *   ([+][2][+])([int][2][1][2][2][int])([cos])([int_short][3])
 *
 *   Comparison:
 *   +------------------+-----------------+------------------|-----------------+
 *   |                  | Option A        | Option B         | Option C        |
 *   |------------------+-----------------+------------------|-----------------|
 *   | Compacity        | ✓               | ✓                | x               |
 *   |                  |                 | 1 bit/block lost | Meta blocks for |
 *   |                  |                 |                  | nodes of size>1 |
 *   |------------------+-----------------+------------------|-----------------|
 *   | Parent retrieve  | x               | ✓                | ✓               |
 *   |                  | Has to scan the | Backward scan    | Backward scan   |
 *   |                  | whole pool      |                  |                 |
 *   |------------------+-----------------+------------------|-----------------|
 *   | Value extraction | ✓               | x                | ✓               |
 *   |                  | Maybe align     | Requires masks   | Maybe align     |
 *   |                  | float/double    |                  | float/double    |
 *   +------------------+-----------------+------------------+-----------------+
 *
 * Optional optimizations:
 * - Align float and double by letting empty blocks
 * - Create special type for common integer INT_ONE or INT_SHORT
 *
 */

namespace Poincare {

/* A block is a byte-long object containing either a type or some value.
 * Several blocks can form a node, like:
 * [INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
 * [ADDITION][LENGTH][ADDITION]
 * A node can also be composed of a single block:
 * [COSINE]
 */

#if __EMSCRIPTEN__
/* Emscripten memory representation assumes loads and stores are aligned.
 * Because the TreePool buffer is going to store double values, Node addresses
 * have to be aligned on 8 bytes (provided that emscripten addresses are 8 bytes
 * long which ensures that v-tables are also aligned). */
typedef uint64_t AlignedNodeBuffer;
#else
/* Memory copies are done quicker on 4 bytes aligned data. We force the TreePool
 * to allocate 4-byte aligned range to leverage this. */
typedef uint32_t AlignedNodeBuffer;
#endif
constexpr static int ByteAlignment = sizeof(AlignedNodeBuffer);

enum class BlockType : uint8_t {
  Ghost = 0,
  IntegerHead,
  IntegerTail,
  IntegerShortHead,
  IntegerShortTail,
  FloatHead,
  FloatTail,
  AdditionHead,
  AdditionTail,
  MultiplicationHead,
  MultiplicationTail
};

class TreeBlock {
friend class TreePool;
public:
  constexpr TreeBlock(uint8_t content = 0) : m_content(content) {}
  bool operator!=(const TreeBlock& b) { return b.m_content != m_content; }

  // Block Navigation
  TreeBlock * nextBlock() { return this + sizeof(TreeBlock); }
  TreeBlock * nextNthBlock(int i) { return this + i * sizeof(TreeBlock); }
  TreeBlock * previousBlock() { return this - sizeof(TreeBlock); }
  TreeBlock * previousNthBlock(int i) { return this - i * sizeof(TreeBlock); }

protected:
  template <typename T>
  class Iterator {
  public:
    Iterator(const T * block) : m_block(const_cast<T *>(block)) {}
    T * operator*() { return m_block; }
    bool operator!=(const Iterator& it) const { return (m_block != it.m_block); }
  protected:
    T * m_block;
  };

  uint8_t m_content;
};

typedef TreeBlock * (TreeBlock::*NextStep)();
typedef TreeBlock * (TreeBlock::*NextNthStep)(int i);

class TypeTreeBlock : public TreeBlock {
public:
  constexpr TypeTreeBlock(BlockType content) : TreeBlock(static_cast<uint8_t>(content)) {}

#if POINCARE_TREE_LOG
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true);
#endif

  // Block Navigation
  TypeTreeBlock * nextNode();
  TypeTreeBlock * previousNode(const TreeBlock * firstBlock);
  TypeTreeBlock * nextSibling();
  TypeTreeBlock * previousSibling(const TreeBlock * firstBlock);

  // Sizes
  size_t nodeSize() const { return const_cast<TypeTreeBlock *>(this)->nextNode() - this; }
  size_t treeSize() const { return const_cast<TypeTreeBlock *>(this)->nextSibling() - this; }

  // Node Hierarchy
  TypeTreeBlock * parent(const TreeBlock * firstBlock);
  TypeTreeBlock * root(const TreeBlock * firstBlock);
  int numberOfChildren() const;
  //void incrementNumberOfChildren(int increment = 1);
  //void eraseNumberOfChildren();
  int numberOfDescendants(bool includeSelf) const;
  TypeTreeBlock * childAtIndex(int i) const;
  int indexOfChild(const TypeTreeBlock * child) const;
  int indexInParent(const TreeBlock * firstBlock) const;
  bool hasChild(const TypeTreeBlock * child) const;
  bool hasAncestor(const TreeBlock * firstBlock, const TypeTreeBlock * node, bool includeSelf) const;
  bool hasSibling(const TreeBlock * firstBlock, const TypeTreeBlock * e) const;

  class ForwardDirect final {
  public:
    ForwardDirect(TypeTreeBlock * block) : m_block(block) {}
    class Iterator : public TreeBlock::Iterator<TypeTreeBlock> {
    public:
      using TreeBlock::Iterator<TypeTreeBlock>::Iterator;
      Iterator & operator++() {
        this->m_block = static_cast<TypeTreeBlock *>(this->m_block)->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_block->nextNode()); }
    Iterator end() const { return Iterator(m_block->nextSibling()); }
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

private:
  TypeTreeBlock * previousRelative(const TreeBlock * firstBlock, bool parent);
  bool isGhost() const { return type() == BlockType::Ghost; }
};

class ValueTreeBlock : public TreeBlock {
public:
  constexpr ValueTreeBlock(uint8_t value) : TreeBlock(value) {}
  uint8_t value() const { return m_content; }
};

static_assert(sizeof(TreeBlock) == 1);
static_assert(sizeof(ValueTreeBlock) == sizeof(TreeBlock));
static_assert(sizeof(TypeTreeBlock) == sizeof(TreeBlock));

constexpr static TypeTreeBlock AdditionHeadBlock() { return TypeTreeBlock(BlockType::AdditionHead); }
constexpr static TypeTreeBlock AdditionTailBlock() { return TypeTreeBlock(BlockType::AdditionTail); }
constexpr static TypeTreeBlock MultiplicationHeadBlock() { return TypeTreeBlock(BlockType::MultiplicationHead); }
constexpr static TypeTreeBlock MultiplicationTailBlock() { return TypeTreeBlock(BlockType::MultiplicationTail); }
constexpr static TypeTreeBlock IntegerHeadBlock() { return TypeTreeBlock(BlockType::IntegerHead); }
constexpr static TypeTreeBlock IntegerTailBlock() { return TypeTreeBlock(BlockType::IntegerTail); }

/*
 * Build pseudo virtuality
 * block --> pointer (switch on type) --> virtual Expression * (static s_expression)
 *
 * class Expression {
public:
  virtual int reduce??
};*/

}

#endif

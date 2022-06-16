#ifndef POINCARE_TREE_BLOCK_H
#define POINCARE_TREE_BLOCK_H

#include <stddef.h>
#include <stdint.h>

namespace Poincare {

enum class BlockType : uint8_t {
  Addition = 100,
  Multiplication = 101,
  Integer = 102
};

class TreeBlock {
public:
  constexpr TreeBlock(uint8_t content = 0) : m_content(content) {}
  constexpr TreeBlock(BlockType content) : m_content(static_cast<uint8_t>(content)) {}

  bool operator!=(const TreeBlock& b) { return b.m_content != m_content; }

  const char * log();

  int numberOfSubtrees() const;
  TreeBlock * nextBlock() { return this + sizeof(TreeBlock); }
  TreeBlock * nextNthBlock(int i) { return this + i * sizeof(TreeBlock); }
  TreeBlock * previousBlock() { return this - sizeof(TreeBlock); }
  TreeBlock * nextTree();

  class Direct final {
  public:
    Direct(const TreeBlock * block) : m_block(const_cast<TreeBlock *>(block)) {}
    class Iterator {
    public:
      Iterator(const TreeBlock * block) : m_block(const_cast<TreeBlock *>(block)) {}
      TreeBlock * operator*() { return m_block; }
      bool operator!=(const Iterator& it) const { return (m_block != it.m_block); }
      Iterator & operator++() {
        this->m_block = this->m_block->nextTree();
        return *this;
      }
    private:
      TreeBlock * m_block;
    };
    Iterator begin() const { return Iterator(m_block->nextBlock()); }
    Iterator end() const { return Iterator(m_block->nextTree()); }
  private:
    TreeBlock * m_block;
  };

  class BackwardsDirect final {
  public:
    BackwardsDirect(const TreeBlock * block);

    class Iterator {
    public:

      class Memoizer {
      public:
        Memoizer(TreeBlock * treeBlock);
        TreeBlock * subtreeAtIndex(int i);
        size_t numberOfSubtrees() { return m_numberOfSubtrees; }
      private:
        void memoizeUntilIndex(int i);
        TreeBlock * m_block;
        // Memoization of subtrees addresses in a RingBuffer
        constexpr static size_t k_maxNumberOfMemoizedSubtrees = 16;
        TreeBlock * m_subtrees[k_maxNumberOfMemoizedSubtrees];
        size_t m_firstMemoizedSubtreeIndex; // Index used for ring buffer
        size_t m_firstSubtreeIndex;
        size_t m_numberOfSubtrees;
      };

      Iterator(int subtreeIndex, Memoizer * memoizer) :
        m_subtreeIndex(subtreeIndex),
        m_memoizer(memoizer) {}
      TreeBlock * operator*() { return m_memoizer->subtreeAtIndex(m_subtreeIndex); }
      bool operator!=(const Iterator& it) const { return (m_subtreeIndex != it.m_subtreeIndex); }

      Iterator & operator++() {
        m_subtreeIndex--;
        return *this;
      }
    private:
      int m_subtreeIndex;
      mutable Memoizer * m_memoizer;
    };
    Iterator begin() const { return Iterator(m_memoizer.numberOfSubtrees() - 1, &m_memoizer); }
    Iterator end() const { return Iterator(-1, &m_memoizer); }
  private:
    mutable Iterator::Memoizer m_memoizer;
  };

  Direct directSubtrees() const { return Direct(this); }
  BackwardsDirect backwardsDirectSubtrees() const { return BackwardsDirect(this); }

  BlockType type() const { return static_cast<BlockType>(m_content); }
  uint8_t value() const { return m_content; }

private:
  uint8_t m_content;
};

static_assert(sizeof(TreeBlock) == 1);

constexpr static TreeBlock AdditionBlock() { return TreeBlock(BlockType::Addition); }
constexpr static TreeBlock MultiplicationBlock() { return TreeBlock(BlockType::Multiplication); }
constexpr static TreeBlock IntegerBlock() { return TreeBlock(BlockType::Integer); }

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


#ifndef POINCARE_TREE_BLOCK_H
#define POINCARE_TREE_BLOCK_H

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
  TreeBlock * previousBlock() { return this - sizeof(TreeBlock); }
  TreeBlock * nextTree();

  class Iterator {
  public:
    Iterator(const TreeBlock * block) : m_block(const_cast<TreeBlock *>(block)) {}
    TreeBlock * operator*() { return m_block; }
    bool operator!=(const Iterator& it) const { return (m_block != it.m_block); }
  protected:
    TreeBlock * m_block;
  };

  class Direct final {
  public:
    Direct(const TreeBlock * block) : m_block(const_cast<TreeBlock *>(block)) {}
    class Iterator : public TreeBlock::Iterator {
    public:
      using TreeBlock::Iterator::Iterator;
      Iterator & operator++() {
        this->m_block = this->m_block->nextTree();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_block->nextBlock()); }
    Iterator end() const { return Iterator(m_block->nextTree()); }
  private:
    TreeBlock * m_block;
  };

  class DepthFirst final {
  public:
    DepthFirst(const TreeBlock * block) : m_block(const_cast<TreeBlock *>(block)) {}
    class Iterator : public TreeBlock::Iterator {
    public:
      using TreeBlock::Iterator::Iterator;
      Iterator & operator++() {
        this->m_block = this->m_block->nextBlock();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_block->nextBlock()); }
    Iterator end() const { return Iterator(m_block->nextBlock()); }
  private:
    TreeBlock * m_block;
  };

  Direct directChildren() const { return Direct(this); }
  DepthFirst depthFirstChildren() const { return DepthFirst(this); }

private:
  uint8_t m_content;
};

static_assert(sizeof(TreeBlock) == 1);

constexpr static TreeBlock AdditionBlock() { return TreeBlock(BlockType::Addition); }
constexpr static TreeBlock MultiplicationBlock() { return TreeBlock(BlockType::Multiplication); }
constexpr static TreeBlock IntegerBlock() { return TreeBlock(BlockType::Integer); }

}

#endif


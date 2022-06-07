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

  int numberOfSubtrees() const;

  const char * log();

private:
  uint8_t m_content;
};

static_assert(sizeof(TreeBlock) == 1);

constexpr static TreeBlock AdditionBlock() { return TreeBlock(BlockType::Addition); }
constexpr static TreeBlock MultiplicationBlock() { return TreeBlock(BlockType::Multiplication); }
constexpr static TreeBlock IntegerBlock() { return TreeBlock(BlockType::Integer); }

}

#endif


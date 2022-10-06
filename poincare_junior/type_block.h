#ifndef POINCARE_TYPE_BLOCK_H
#define POINCARE_TYPE_BLOCK_H

#include "block.h"

namespace Poincare {

enum class BlockType : uint8_t {
#if GHOST_REQUIRED
  Ghost,
#endif
// InternalExpression
  Integer,
  IntegerShort,
  Float,
  Addition,
  Multiplication,
  Power,
  Constant,
// Expression
  Subtraction,
  Division
};

// TODO:
// - if the number of BlockType > 256, add a special tag that prefixes the least
//   used tags
// - Optimization: some Integer should have their special tags? 0, 1, 2, 10?

class TypeBlock : public Block {
public:
  constexpr TypeBlock(BlockType content = BlockType::Integer) : Block(static_cast<uint8_t>(content)) {}
  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }

private:
#if GHOST_REQUIRED
  bool isGhost() const { return type() == BlockType::Ghost; }
#endif
};

static_assert(sizeof(TypeBlock) == sizeof(Block));

constexpr TypeBlock AdditionBlock = TypeBlock(BlockType::Addition);
constexpr TypeBlock MultiplicationBlock = TypeBlock(BlockType::Multiplication);
constexpr TypeBlock IntegerBlock = TypeBlock(BlockType::Integer);
constexpr TypeBlock SubtractionBlock = TypeBlock(BlockType::Subtraction);
constexpr TypeBlock DivisionBlock = TypeBlock(BlockType::Division);
constexpr TypeBlock PowerBlock = TypeBlock(BlockType::Power);
constexpr TypeBlock ConstantBlock = TypeBlock(BlockType::Constant);

}

#endif

#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include "block.h"

namespace Poincare {

enum class BlockType : uint8_t {
// InternalExpression
  Zero = 0,
  One = 1,
  Two = 2,
  Half,
  MinusOne,
  // TODO: add 180, 200?
  IntegerShort,
  IntegerPosBig,
  IntegerNegBig,
  RationalShort,
  RationalPosBig,
  RationalNegBig,
  Float,
  Addition,
  Multiplication,
  Power,
  Constant,
// Expression
  Subtraction,
  Division,
  NumberOfExpressions,

  HorizontalLayout = NumberOfExpressions,
  NumberOfTypes
};

// TODO:
// - if the number of BlockType > 256, add a special tag that prefixes the least
//   used tags
// - Optimization: some Integer should have their special tags? 0, 1, 2, 10?

class TypeBlock : public Block {
public:
  constexpr TypeBlock(BlockType content = BlockType::Zero) : Block(static_cast<uint8_t>(content)) {}
  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }
  bool isOfType(std::initializer_list<BlockType> types) const;

  bool isInteger() const { return isOfType({BlockType::Zero, BlockType::One, BlockType::Two, BlockType::Half, BlockType::MinusOne, BlockType::IntegerShort, BlockType::IntegerPosBig, BlockType::IntegerNegBig}); }
  bool isRational() const { return isOfType({BlockType::RationalShort, BlockType::RationalPosBig, BlockType::RationalNegBig}) || isInteger(); }
  bool isNumber() const { return isOfType({BlockType::Float}) || isRational(); }
  bool isExpression() const { return m_content < static_cast<uint8_t>(BlockType::NumberOfExpressions); }

};

static_assert(sizeof(TypeBlock) == sizeof(Block));

constexpr TypeBlock ZeroBlock = TypeBlock(BlockType::Zero);
constexpr TypeBlock OneBlock = TypeBlock(BlockType::One);
constexpr TypeBlock TwoBlock = TypeBlock(BlockType::Two);
constexpr TypeBlock HalfBlock = TypeBlock(BlockType::Half);
constexpr TypeBlock MinusOne = TypeBlock(BlockType::MinusOne);
constexpr TypeBlock IntegerShortBlock = TypeBlock(BlockType::IntegerShort);
constexpr TypeBlock IntegerPosBigBlock = TypeBlock(BlockType::IntegerPosBig);
constexpr TypeBlock IntegerNegBigBlock = TypeBlock(BlockType::IntegerNegBig);
constexpr TypeBlock RationalShortBlock = TypeBlock(BlockType::RationalShort);
constexpr TypeBlock RationalPosBigBlock = TypeBlock(BlockType::RationalPosBig);
constexpr TypeBlock RationalNegBigBlock = TypeBlock(BlockType::RationalNegBig);
constexpr TypeBlock FloatBlock = TypeBlock(BlockType::Float);
constexpr TypeBlock AdditionBlock = TypeBlock(BlockType::Addition);
constexpr TypeBlock MultiplicationBlock = TypeBlock(BlockType::Multiplication);
constexpr TypeBlock PowerBlock = TypeBlock(BlockType::Power);
constexpr TypeBlock ConstantBlock = TypeBlock(BlockType::Constant);
constexpr TypeBlock SubtractionBlock = TypeBlock(BlockType::Subtraction);
constexpr TypeBlock DivisionBlock = TypeBlock(BlockType::Division);

}

#endif

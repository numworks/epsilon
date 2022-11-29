#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include "block.h"

namespace Poincare {

  /* Node description by type:
   * - IntegerShort
   * | INTEGER_SHORT TAG | SIGNED DIGIT0 | INTEGER_SHORT TAG |
   *
   *  - Integer(Pos/Neg)Big
   *  | INTEGER_(POS/NEG)_BIG TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | NUMBER DIGITS | INTEGER_(POS/NEG)_BIG TAG TAG |
   *
   * - RationShort
   * | RATIONAL_SHORT TAG | SIGNED DIGIT | UNSIGNED DIGIT | RATIONAL_SHORT TAG |
   *
   * - Rational(Pos/Neg)Big
   * | RATIONAL_(POS/NEG)_BIG TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | NUMBER DIGITS | RATIONAL_(POS/NEG)_BIG TAG TAG |
   *
   * */

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
  NumberOfNumbersExpression,

  Constant = NumberOfNumbersExpression,
  Addition,
  Multiplication,
  Power,
  Factorial,
  UserSymbol,
  UserFunction,
  UserSequence,
// Expression
  Subtraction,
  Division,
  NumberOfExpressions,

  HorizontalLayout = NumberOfExpressions,
  NumberOfTypes
};

#define BLOCK_TYPE_IS_EXPRESSION_NUMBER(type) static_assert(type >= static_cast<BlockType>(0) && type < BlockType::NumberOfNumbersExpression);
#define BLOCK_TYPE_IS_EXPRESSION(type) static_assert(type >= BlockType::NumberOfNumbersExpression && type < BlockType::NumberOfExpressions);
#define BLOCK_TYPE_IS_LAYOUT(type) static_assert(type >= BlockType::NumberOfExpressions && type < BlockType::NumberOfTypes);

BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::Zero);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::One);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::Two);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::Half);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::MinusOne);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::IntegerShort);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::IntegerPosBig);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::IntegerNegBig);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::RationalShort);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::RationalPosBig);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::RationalNegBig);
BLOCK_TYPE_IS_EXPRESSION_NUMBER(BlockType::Float);

BLOCK_TYPE_IS_EXPRESSION(BlockType::Addition);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Multiplication);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Power);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Constant);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Subtraction);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Division);

BLOCK_TYPE_IS_LAYOUT(BlockType::HorizontalLayout);

// TODO:
// - if the number of BlockType > 256, add a special tag that prefixes the least
//   used tags
// - Optimization: some Integer should have their special tags? 0, 1, 2, 10?

class TypeBlock : public Block {
public:
  constexpr TypeBlock(BlockType content = BlockType::Zero) : Block(static_cast<uint8_t>(content)) {
    // assert that number are always sorted before other types
    assert(isNumber() || m_content >= static_cast<uint8_t>(BlockType::NumberOfNumbersExpression));
  }
  constexpr BlockType type() const {
    return static_cast<BlockType>(m_content);
  }

  constexpr bool isExpression() const { return m_content < static_cast<uint8_t>(BlockType::NumberOfExpressions); }

  constexpr bool isOfType(std::initializer_list<BlockType> types) const {
    BlockType thisType = type();
    for (BlockType t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isInteger() const { return isOfType({BlockType::Zero, BlockType::One, BlockType::Two, BlockType::Half, BlockType::MinusOne, BlockType::IntegerShort, BlockType::IntegerPosBig, BlockType::IntegerNegBig}); }
  constexpr bool isRational() const { return isOfType({BlockType::RationalShort, BlockType::RationalPosBig, BlockType::RationalNegBig}) || isInteger(); }
  constexpr bool isNumber() const { return isOfType({BlockType::Float}) || isRational(); }
  constexpr bool isUserNamed() const { return isOfType({BlockType::UserFunction, BlockType::UserSequence, BlockType::UserSymbol}); }

  constexpr static size_t NumberOfMetaBlocks(BlockType type) {
    switch (type) {
      case BlockType::IntegerShort:
        return 3;
      case BlockType::IntegerPosBig:
      case BlockType::IntegerNegBig:
        return 4;
      case BlockType::RationalShort:
        return 4;
      case BlockType::RationalPosBig:
      case BlockType::RationalNegBig:
        return 5;
      case BlockType::Float:
        return 2 + sizeof(float)/sizeof(uint8_t);
      case BlockType::Addition:
      case BlockType::Multiplication:
        return 3;
      case BlockType::Constant:
        return 3;
      default:
        return 1;
    };
  }

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

#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include "block.h"
#include <ion/unicode/code_point.h>

namespace PoincareJ {

/* TODO:
 * - Are Zero, One etc useful?
 * - Short integers could be coded on n-bytes (with n static) instead of 1-byte. Choosing n = 4 and aligning the node could be useful?
 * - aligning all nodes on 4 bytes might speed up every computation
*/

/* Node description by type:
 * - Zero Z (same for One, Two, Half, MinusOne, NodeBorder)
 * | Z TAG |
 *
 * - IntegerShort IS
 * | IS TAG | SIGNED DIGIT0 | IS TAG |
 *
 * - Integer(Pos/Neg)Big IB: most significant digit last
 * | IB TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | NUMBER DIGITS | IB |
 *
 * - RationShort RS
 * | RS TAG | SIGNED DIGIT | UNSIGNED DIGIT | RS TAG |
 *
 * - Rational(Pos/Neg)Big RB
 * | RB TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | NUMBER DIGITS | RB |
 *
 * - Float F (same for CodePointLayout)
 * | F TAG | VALUE (4 bytes) | F TAG |
 *
 * - Constant C
 * | C TAG | TYPE | C TAG |
 *
 * - Addition A (same for Multiplication, Set, List, RackLayout)
 * | A TAG | NUMBER OF CHILDREN | A TAG |
 *
 * - Power P (same for Factorial, Subtraction, Division, FractionLayout, ParenthesisLayout, VerticalOffsetLayout)
 * | P TAG |
 *
 * - UserSymbol US (same for UserFunction, UserSequence)
 * | US TAG | NUMBER CHARS | CHAR0 | ... | CHARN | NUMBER CHARS | US TAG |
 *
 * - Polynomial P = a0*x^e0 + ... + a1*x^e1 + ... +
 *   n = number of terms
 *  | P TAG | n | e0 | e1 | ... | n | P TAG |
 *  This node has n + 1 children:
 *  - the first child describes the variable x
 *  - the n following children describe the coefficients.
 *  Polynomials can be recursive (have polynomials children)
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
  Set,
  List,
  Polynomial,
  NumberOfExpressions,
  // Layout
  FirstLayout = NumberOfExpressions,
  RackLayout = FirstLayout,
  FractionLayout,
  ParenthesisLayout,
  VerticalOffsetLayout,
  CodePointLayout,
  LastLayout = CodePointLayout,
  NumberOfLayouts,
  // Misc
  NodeBorder = NumberOfLayouts,
  Placeholder,
  NumberOfTypes
};

#define BLOCK_TYPE_IS_EXPRESSION_NUMBER(type) static_assert(type >= static_cast<BlockType>(0) && type < BlockType::NumberOfNumbersExpression);
#define BLOCK_TYPE_IS_EXPRESSION(type) static_assert(type >= BlockType::NumberOfNumbersExpression && type < BlockType::NumberOfExpressions);
#define BLOCK_TYPE_IS_LAYOUT(type) static_assert(type >= BlockType::FirstLayout && type <= BlockType::LastLayout);

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

BLOCK_TYPE_IS_EXPRESSION(BlockType::Constant);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Addition);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Multiplication);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Power);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Factorial);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserSymbol);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserFunction);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserSequence);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Subtraction);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Division);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Set);
BLOCK_TYPE_IS_EXPRESSION(BlockType::List);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Polynomial);

BLOCK_TYPE_IS_LAYOUT(BlockType::RackLayout);
BLOCK_TYPE_IS_LAYOUT(BlockType::FractionLayout);
BLOCK_TYPE_IS_LAYOUT(BlockType::ParenthesisLayout);
BLOCK_TYPE_IS_LAYOUT(BlockType::VerticalOffsetLayout);
BLOCK_TYPE_IS_LAYOUT(BlockType::CodePointLayout);

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
  constexpr bool isLayout() const { return m_content >= static_cast<uint8_t>(BlockType::NumberOfExpressions) && m_content < static_cast<uint8_t>(BlockType::NumberOfLayouts); }

  constexpr bool isOfType(std::initializer_list<BlockType> types) const {
    BlockType thisType = type();
    for (BlockType t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isNAry() const { return isOfType({BlockType::Addition, BlockType::Multiplication, BlockType::RackLayout, BlockType::Set, BlockType::List, BlockType::Polynomial}); }
  constexpr bool isInteger() const { return isOfType({BlockType::Zero, BlockType::One, BlockType::Two, BlockType::MinusOne, BlockType::IntegerShort, BlockType::IntegerPosBig, BlockType::IntegerNegBig}); }
  constexpr bool isRational() const { return isOfType({BlockType::Half, BlockType::RationalShort, BlockType::RationalPosBig, BlockType::RationalNegBig}) || isInteger(); }
  constexpr bool isNumber() const { return isOfType({BlockType::Float}) || isRational(); }
  constexpr bool isUserNamed() const { return isOfType({BlockType::UserFunction, BlockType::UserSequence, BlockType::UserSymbol}); }

  constexpr static size_t NumberOfMetaBlocks(BlockType type) {
    switch (type) {
      case BlockType::IntegerShort:
      case BlockType::Placeholder:
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
      case BlockType::CodePointLayout:
        return 2 + sizeof(CodePoint)/sizeof(uint8_t);
      case BlockType::Addition:
      case BlockType::Multiplication:
      case BlockType::Constant:
      case BlockType::Set:
      case BlockType::List:
      case BlockType::RackLayout:
        return 3;
      case BlockType::Polynomial:
      case BlockType::UserSymbol:
      case BlockType::UserFunction:
      case BlockType::UserSequence:
        return 4;
      default:
        return 1;
    };
  }

};

static_assert(sizeof(TypeBlock) == sizeof(Block));

constexpr TypeBlock ZeroBlock = TypeBlock(BlockType::Zero);
constexpr TypeBlock OneBlock = TypeBlock(BlockType::One);
constexpr TypeBlock TwoBlock = TypeBlock(BlockType::Two);
constexpr TypeBlock MinusOneBlock = TypeBlock(BlockType::MinusOne);
constexpr TypeBlock HalfBlock = TypeBlock(BlockType::Half);
constexpr TypeBlock NodeBorderBlock = TypeBlock(BlockType::NodeBorder);

}

#endif

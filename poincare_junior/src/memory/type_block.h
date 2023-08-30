#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include <ion/unicode/code_point.h>

#include "block.h"

namespace PoincareJ {

/* The items to include in the enum are wrapped with a macro and split in
 * different files to tidy them and be able to use them in different ways (in
 * Tree::log for instance). */

enum class BlockType : uint8_t {
// Add all the types to the enum
#define TYPE(F) F,
#define ALIAS(F)
#include <poincare_junior/src/memory/block_types.h>
#undef TYPE
#undef ALIAS

// Add all the aliases after the types (for them not to increment the tags)
#define TYPE(F)
#define ALIAS(F) F,
#include <poincare_junior/src/memory/block_types.h>
#undef TYPE
#undef ALIAS
};

#define BLOCK_TYPE_IS_EXPRESSION_NUMBER(type)        \
  static_assert(type >= static_cast<BlockType>(0) && \
                type < BlockType::NumberOfNumbersExpression);
#define BLOCK_TYPE_IS_EXPRESSION(type)                          \
  static_assert(type >= BlockType::NumberOfNumbersExpression && \
                type < BlockType::NumberOfExpressions);
#define BLOCK_TYPE_IS_LAYOUT(type)                \
  static_assert(type >= BlockType::FirstLayout && \
                type <= BlockType::LastLayout);

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
BLOCK_TYPE_IS_EXPRESSION(BlockType::PowerReal);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Abs);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Cosine);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Sine);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Tangent);
BLOCK_TYPE_IS_EXPRESSION(BlockType::ArcCosine);
BLOCK_TYPE_IS_EXPRESSION(BlockType::ArcSine);
BLOCK_TYPE_IS_EXPRESSION(BlockType::ArcTangent);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Complex);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Logarithm);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Log);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Ln);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Exponential);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Trig);
BLOCK_TYPE_IS_EXPRESSION(BlockType::TrigDiff);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Factorial);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserSymbol);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserFunction);
BLOCK_TYPE_IS_EXPRESSION(BlockType::UserSequence);
BLOCK_TYPE_IS_EXPRESSION(BlockType::SquareRoot);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Subtraction);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Division);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Set);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Matrix);
BLOCK_TYPE_IS_EXPRESSION(BlockType::List);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Polynomial);
BLOCK_TYPE_IS_EXPRESSION(BlockType::Derivative);

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
  constexpr TypeBlock(BlockType content = BlockType::Zero)
      : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(BlockType::NumberOfTypes));
    // assert that number are always sorted before other types
    assert(isNumber() ||
           m_content >=
               static_cast<uint8_t>(BlockType::NumberOfNumbersExpression));
  }
  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }

  constexpr bool isExpression() const {
    return m_content < static_cast<uint8_t>(BlockType::NumberOfExpressions);
  }
  constexpr bool isAlgebraic() const {
    return isNumber() ||
           isOfType({BlockType::Addition, BlockType::Multiplication,
                     BlockType::Power});
  }
  constexpr bool isLayout() const {
    return m_content >= static_cast<uint8_t>(BlockType::NumberOfExpressions) &&
           m_content < static_cast<uint8_t>(BlockType::NumberOfLayouts);
  }

  constexpr bool isOfType(std::initializer_list<BlockType> types) const {
    BlockType thisType = type();
    for (BlockType t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isNAry() const {
    return isOfType({BlockType::Addition, BlockType::Multiplication,
                     BlockType::RackLayout, BlockType::Set, BlockType::List,
                     BlockType::Polynomial, BlockType::SystemList});
  }
  // NAry with a single metaBlock for number of children
  constexpr bool isSimpleNAry() const {
    return isNAry() && nodeSize() == NumberOfMetaBlocks(type());
  }
  constexpr bool isInteger() const {
    return isOfType({BlockType::Zero, BlockType::One, BlockType::Two,
                     BlockType::MinusOne, BlockType::IntegerShort,
                     BlockType::IntegerPosBig, BlockType::IntegerNegBig});
  }
  constexpr bool isRational() const {
    return isOfType({BlockType::Half, BlockType::RationalShort,
                     BlockType::RationalPosBig, BlockType::RationalNegBig}) ||
           isInteger();
  }
  // TODO: Handle complex numbers
  constexpr bool isNumber() const {
    return type() == BlockType::Float || isRational() ||
           type() == BlockType::Constant;
  }
  constexpr bool isUserNamed() const {
    return isOfType({BlockType::UserFunction, BlockType::UserSequence,
                     BlockType::UserSymbol});
  }

  constexpr static size_t NumberOfMetaBlocks(BlockType type) {
    switch (type) {
      case BlockType::Float:
        return 1 + sizeof(float) / sizeof(uint8_t);
      case BlockType::CodePointLayout:
        return 1 + sizeof(CodePoint) / sizeof(uint8_t);
      case BlockType::Addition:
      case BlockType::Multiplication:
      case BlockType::Constant:
      case BlockType::Set:
      case BlockType::List:
      case BlockType::RackLayout:
      case BlockType::SystemList:
      case BlockType::Polynomial:
      case BlockType::UserSymbol:
      case BlockType::UserFunction:
      case BlockType::UserSequence:
      case BlockType::Decimal:
      case BlockType::IntegerShort:
      case BlockType::Placeholder:
      case BlockType::IntegerPosBig:
      case BlockType::IntegerNegBig:
        return 2;
      case BlockType::RationalShort:
      case BlockType::RationalPosBig:
      case BlockType::RationalNegBig:
      case BlockType::Matrix:
        return 3;
      default:
        return 1;
    };
  }

  constexpr size_t nodeSize() const {
    BlockType t = type();
    size_t numberOfMetaBlocks = NumberOfMetaBlocks(t);
    switch (t) {
      case BlockType::IntegerPosBig:
      case BlockType::IntegerNegBig: {
        uint8_t numberOfDigits = static_cast<uint8_t>(*next());
        return numberOfMetaBlocks + numberOfDigits;
      }
      case BlockType::RationalPosBig:
      case BlockType::RationalNegBig: {
        uint8_t numberOfDigitsNumerator = static_cast<uint8_t>(*next());
        uint8_t numberOfDigitsDenominator = static_cast<uint8_t>(*nextNth(2));
        return numberOfMetaBlocks +
               static_cast<size_t>(numberOfDigitsNumerator) +
               static_cast<size_t>(numberOfDigitsDenominator);
      }
      case BlockType::Polynomial: {
        uint8_t numberOfTerms = static_cast<uint8_t>(*next()) - 1;
        return numberOfMetaBlocks + numberOfTerms;
      }
      case BlockType::UserSymbol: {
        uint8_t numberOfChars = static_cast<uint8_t>(*next());
        return numberOfMetaBlocks + numberOfChars;
      }
      default:
        return numberOfMetaBlocks;
    }
  }

  constexpr int numberOfChildren() const {
    if (isNAry()) {
      return static_cast<uint8_t>(*next());
    }
    switch (type()) {
      case BlockType::Matrix:
        return static_cast<uint8_t>(*next()) *
               static_cast<uint8_t>(*nextNth(2));
      case BlockType::Derivative:
        return 3;
      case BlockType::Power:
      case BlockType::PowerReal:
      case BlockType::PowerMatrix:
      case BlockType::Subtraction:
      case BlockType::Complex:
      case BlockType::Division:
      case BlockType::FractionLayout:
      case BlockType::Trig:
      case BlockType::TrigDiff:
      case BlockType::Logarithm:
      case BlockType::Cross:
      case BlockType::Dot:
        return 2;
      case BlockType::Abs:
      case BlockType::Decimal:
      case BlockType::Cosine:
      case BlockType::Sine:
      case BlockType::Tangent:
      case BlockType::ArcCosine:
      case BlockType::ArcSine:
      case BlockType::ArcTangent:
      case BlockType::Log:
      case BlockType::Ln:
      case BlockType::Exponential:
      case BlockType::Factorial:
      case BlockType::Det:
      case BlockType::Dim:
      case BlockType::Identity:
      case BlockType::Inverse:
      case BlockType::Norm:
      case BlockType::Ref:
      case BlockType::Rref:
      case BlockType::Trace:
      case BlockType::Transpose:
      case BlockType::ParenthesisLayout:
      case BlockType::SquareRoot:
      case BlockType::VerticalOffsetLayout:
        return 1;
      default:
        return 0;
    }
  }

  bool isScalarOnly() const {
    return !(BlockType::FirstMatrix <= type() &&
             type() <= BlockType::LastMatrix);
  }
};

static_assert(sizeof(TypeBlock) == sizeof(Block));

constexpr TypeBlock ZeroBlock = TypeBlock(BlockType::Zero);
constexpr TypeBlock OneBlock = TypeBlock(BlockType::One);
constexpr TypeBlock TwoBlock = TypeBlock(BlockType::Two);
constexpr TypeBlock MinusOneBlock = TypeBlock(BlockType::MinusOne);
constexpr TypeBlock HalfBlock = TypeBlock(BlockType::Half);
#if ASSERTIONS
constexpr TypeBlock TreeBorderBlock = TypeBlock(BlockType::TreeBorder);
#endif

// Add a TreeBorder blocks at the end to assert we don't navigate out of it.
template <int size>
class BlockBuffer {
 public:
  constexpr BlockBuffer() {
#if ASSERTIONS
    m_blocks[size] = TreeBorderBlock;
#endif
  }
  constexpr TypeBlock *blocks() {
    return static_cast<TypeBlock *>(static_cast<Block *>(m_blocks));
  }
  consteval const TypeBlock *blocks() const {
    return static_cast<const TypeBlock *>(m_blocks);
  }

 private:
#if ASSERTIONS
  Block m_blocks[size + 1];
#else
  Block m_blocks[size];
#endif
};

}  // namespace PoincareJ

#endif

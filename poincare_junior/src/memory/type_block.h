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
#define TYPE(F) SCOPED_TYPE(F),
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

enum class LayoutType : uint8_t {
#define TYPE(F) F = static_cast<uint8_t>(BlockType::F##Layout),
#define ALIAS(F)
#include <poincare_junior/src/layout/block_types.h>
#undef TYPE
#undef ALIAS
};

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

  bool operator==(const TypeBlock &other) const = default;
  constexpr bool operator==(BlockType t) const { return type() == t; }
  constexpr operator BlockType() const { return type(); }

  constexpr bool isExpression() const {
    return m_content < static_cast<uint8_t>(BlockType::NumberOfExpressions);
  }
  constexpr bool isAlgebraic() const {
    return isNumber() ||
           isOfType({BlockType::Addition, BlockType::Multiplication,
                     BlockType::Power});
  }
  constexpr bool isParametric() const {
    return isOfType({BlockType::Sum, BlockType::Product, BlockType::Derivative,
                     BlockType::Integral});
  }
  constexpr bool isLayout() const {
    return m_content >= static_cast<uint8_t>(BlockType::NumberOfExpressions) &&
           m_content < static_cast<uint8_t>(BlockType::NumberOfLayouts);
  }

  constexpr static bool IsOfType(BlockType thisType,
                                 std::initializer_list<BlockType> types) {
    for (BlockType t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isOfType(std::initializer_list<BlockType> types) const {
    return IsOfType(type(), types);
  }

  static constexpr std::initializer_list<BlockType> k_nAryBlockTypes = {
      BlockType::Addition,  BlockType::Multiplication, BlockType::RackLayout,
      BlockType::Set,       BlockType::List,           BlockType::Polynomial,
      BlockType::SystemList};

  // Their next metaBlock contains the numberOfChildren
  constexpr static bool IsNAry(BlockType thisType) {
    return IsOfType(thisType, k_nAryBlockTypes);
  }
  constexpr bool isNAry() const { return IsNAry(type()); }
  // NAry with a single metaBlock
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
    // NOTE: Make sure new BlockTypes are handled here.
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
      case BlockType::Variable:
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
      case BlockType::Unit:
        return 3;
      default:
        return 1;
    };
  }

  constexpr size_t nodeSize() const {
    BlockType t = type();
    size_t numberOfMetaBlocks = NumberOfMetaBlocks(t);
    // NOTE: Make sure new BlockTypes are handled here.
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

  constexpr static int NumberOfChildren(BlockType type) {
    assert(type != BlockType::Matrix && !IsNAry(type));
    // NOTE: Make sure new BlockTypes are handled here.
    switch (type) {
      case BlockType::Sum:
      case BlockType::Product:
      case BlockType::Integral:
        return 4;
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
      case BlockType::Dependency:
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
      case BlockType::ComplexArgument:
      case BlockType::Conjugate:
      case BlockType::ImaginaryPart:
      case BlockType::RealPart:
      case BlockType::ParenthesisLayout:
      case BlockType::SquareRoot:
      case BlockType::VerticalOffsetLayout:
        return 1;
      default:
        return 0;
    }
  }

  constexpr int numberOfChildren() const {
    if (isNAry()) {
      return static_cast<uint8_t>(*next());
    }
    BlockType thisType = type();
    if (thisType == BlockType::Matrix) {
      return static_cast<uint8_t>(*next()) * static_cast<uint8_t>(*nextNth(2));
    }
    return NumberOfChildren(thisType);
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

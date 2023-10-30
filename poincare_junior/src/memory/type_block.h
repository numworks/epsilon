#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include <ion/unicode/code_point.h>

#include "block.h"

namespace PoincareJ {

/* The items to include in the enum are wrapped with a macro and split in
 * different files to tidy them and be able to use them in different ways. */

/* The RANGE macro does nothing yet */
#define RANGE(NAME, FIRST, LAST)

#define NARY NAryNumberOfChildrenTag
#define NARY2D NAry2DNumberOfChildrenTag

enum class BlockType : uint8_t {
// Add all the types to the enum
#define NODE(F, N) SCOPED_NODE(F),
#include <poincare_junior/src/memory/types.h>
#undef NODE
};

enum class LayoutType : uint8_t {
// Members of LayoutType have the same values as their BlockType counterpart
#define NODE(F, N) F = static_cast<uint8_t>(BlockType::F##Layout),
#include <poincare_junior/src/layout/types.h>
#undef NODE
};

class TypeBlock : public Block {
 public:
  constexpr TypeBlock(BlockType content = BlockType::Zero)
      : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(BlockType::NumberOfTypes));
  }
  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }

  bool operator==(const TypeBlock &other) const = default;
  constexpr bool operator==(BlockType t) const { return type() == t; }
  constexpr operator BlockType() const { return type(); }

#if POINCARE_MEMORY_TREE_LOG
  // Add an array of names for the BlockTypes
  static constexpr const char *names[] = {
#define NODE(F, N) #F,
#include <poincare_junior/src/memory/types.h>
#undef NODE
  };
#endif

  // Add methods like IsNumber(type) and .isNumber to test range membership
#define NODE(F, N)
#undef RANGE
#define RANGE(NAME, FIRST, LAST)                                \
  static constexpr bool Is##NAME(BlockType type) {              \
    static_assert(BlockType::FIRST < BlockType::LAST);          \
    return BlockType::FIRST <= type && type <= BlockType::LAST; \
  }                                                             \
                                                                \
  constexpr bool is##NAME() const { return Is##NAME(type()); }
#include <poincare_junior/src/memory/types.h>
#undef RANGE
#define RANGE(NAME, FIRST, LAST)
#undef NODE

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

  bool isScalarOnly() const { return !isAMatrixOrContainsMatricesAsChildren(); }

  // Their next metaBlock contains the numberOfChildren
  constexpr static bool IsNAry(BlockType type) {
    return NumberOfChildrenOrTag(type) == NAryNumberOfChildrenTag;
  }
  constexpr bool isNAry() const { return IsNAry(type()); }
  // NAry with a single metaBlock
  constexpr bool isSimpleNAry() const {
    return isNAry() && nodeSize() == NumberOfMetaBlocks(type());
  }

  constexpr static size_t NumberOfMetaBlocks(BlockType type) {
    // NOTE: Make sure new BlockTypes are handled here.
    switch (type) {
      case BlockType::DoubleFloat:
        return 1 + sizeof(double) / sizeof(uint8_t);
      case BlockType::SingleFloat:
        return 1 + sizeof(float) / sizeof(uint8_t);
      case BlockType::CodePointLayout:
        return 1 + sizeof(CodePoint) / sizeof(uint8_t);
      case BlockType::RationalShort:
      case BlockType::RationalPosBig:
      case BlockType::RationalNegBig:
      case BlockType::Matrix:
      case BlockType::Unit:
        return 3;
      case BlockType::Addition:
      case BlockType::Multiplication:
      case BlockType::Constant:
      case BlockType::Set:
      case BlockType::List:
      case BlockType::RackLayout:
      case BlockType::SystemList:
      case BlockType::Polynomial:
      case BlockType::GCD:
      case BlockType::LCM:
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

  constexpr int numberOfChildren() const {
    int n = NumberOfChildrenOrTag(type());
    if (n >= 0) {
      return n;
    } else if (n == NAryNumberOfChildrenTag) {
      return static_cast<uint8_t>(*next());
    } else {
      assert(n == NAry2DNumberOfChildrenTag);
      return static_cast<uint8_t>(*next()) * static_cast<uint8_t>(*nextNth(2));
    }
  }

  constexpr static int NumberOfChildren(BlockType type) {
    assert(type != BlockType::Matrix && !IsNAry(type));
    return NumberOfChildrenOrTag(type);
  }

 private:
  constexpr static int NAryNumberOfChildrenTag = -1;
  constexpr static int NAry2DNumberOfChildrenTag = -2;

  constexpr static int NumberOfChildrenOrTag(BlockType type) {
    switch (type) {
#define NODE(F, N)                \
  case BlockType::SCOPED_NODE(F): \
    return N;
#include <poincare_junior/src/memory/types.h>
#undef NODE
      default:
        return 0;
    }
  }
};

#undef NARY
#undef NARY2D
#undef RANGE

static_assert(sizeof(TypeBlock) == sizeof(Block));

// Add a TreeBorder blocks at the end to assert we don't navigate out of it.
template <int size>
class BlockBuffer {
 public:
  constexpr BlockBuffer() {
#if ASSERTIONS
    m_blocks[size] = BlockType::TreeBorder;
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

// Helper from T  = float|double to corresponding BlockType
template <class T>
struct FloatType;

template <>
struct FloatType<float> {
  static constexpr BlockType type = BlockType::SingleFloat;
};

template <>
struct FloatType<double> {
  static constexpr BlockType type = BlockType::DoubleFloat;
};

}  // namespace PoincareJ

#endif

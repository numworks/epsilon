#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include <ion/unicode/code_point.h>

#include "block.h"

namespace PoincareJ {

/* The items to include in the enum are wrapped with a macro and split in
 * different files to tidy them and be able to use them in different ways. */

/* The macro NODE(name, nbChildren=0, nbAdditionalHeaderBlocks=0) declares nodes
 * with arity nbChildren (can be NARY) and some optional extra blocks in the
 * header to store special data. */

/* The macro RANGE(name, start, end) creates a named group of previously
 * declared nodes. */

#define NARY NAryNumberOfChildrenTag
#define NARY2D NAry2DNumberOfChildrenTag

// Boilerplate to alias NODE(F) as NODE3(F, 0, 0), NODE3 is the varying macro
#define GET4TH(A, B, C, D, ...) D
#define NODE1(F) NODE3(F, 0, 0)
#define NODE2(F, N) NODE3(F, N, 0)
#define NODE(...) GET4TH(__VA_ARGS__, NODE3, NODE2, NODE1)(__VA_ARGS__)

enum class BlockType : uint8_t {
// Add all the types to the enum
#define RANGE(...)
#define NODE3(F, N, S) SCOPED_NODE(F),
#include <poincare_junior/src/memory/types.h>
#undef NODE3
#undef RANGE
};

enum class LayoutType : uint8_t {
// Members of LayoutType have the same values as their BlockType counterpart
#define RANGE(...)
#define NODE3(F, N, S) F = static_cast<uint8_t>(BlockType::F##Layout),
#include <poincare_junior/src/layout/types.h>
#undef NODE3
#undef RANGE
};

class TypeBlock : public Block {
 public:
  constexpr TypeBlock(BlockType content)
      : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(BlockType::NumberOfTypes));
  }
  constexpr BlockType type() const { return static_cast<BlockType>(m_content); }

  bool operator==(const TypeBlock &other) const = default;
  constexpr bool operator==(BlockType t) const { return type() == t; }
  bool operator!=(const TypeBlock &other) const = default;
  constexpr bool operator!=(BlockType t) const { return type() != t; }
  constexpr operator BlockType() const { return type(); }

#if POINCARE_MEMORY_TREE_LOG
  // Add an array of names for the BlockTypes
  static constexpr const char *names[] = {
#define RANGE(...)
#define NODE3(F, N, S) #F,
#include <poincare_junior/src/memory/types.h>
#undef NODE3
#undef RANGE
  };
#endif

  // Add methods like IsNumber(type) and .isNumber to test range membership
#define RANGE(NAME, FIRST, LAST)                                \
  static constexpr bool Is##NAME(BlockType type) {              \
    static_assert(BlockType::FIRST <= BlockType::LAST);         \
    return BlockType::FIRST <= type && type <= BlockType::LAST; \
  }                                                             \
                                                                \
  constexpr bool is##NAME() const { return Is##NAME(type()); }

#define RANGE1(N) RANGE(N, N, N)

#define NODE3(F, N, S) RANGE1(SCOPED_NODE(F))

#include <poincare_junior/src/memory/types.h>
#undef NODE3
#undef RANGE1
#undef RANGE

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

 private:
  constexpr static int NAryNumberOfChildrenTag = -1;
  constexpr static int NAry2DNumberOfChildrenTag = -2;

  consteval static size_t DefaultNumberOfMetaBlocks(int N) {
    return N == NAry2DNumberOfChildrenTag ? 3
           : N == NAryNumberOfChildrenTag ? 2
                                          : 1;
  }

 public:
  constexpr static size_t NumberOfMetaBlocks(BlockType type) {
    switch (type) {
#define RANGE(...)
#define NODE3(F, N, S)            \
  case BlockType::SCOPED_NODE(F): \
    return DefaultNumberOfMetaBlocks(N) + S;
#include <poincare_junior/src/memory/types.h>
      default:
        return 1;
    }
  }
#undef NODE3
#undef RANGE

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
    assert(NumberOfChildrenOrTag(type) != NAryNumberOfChildrenTag &&
           NumberOfChildrenOrTag(type) != NAry2DNumberOfChildrenTag);
    return NumberOfChildrenOrTag(type);
  }

 private:
  constexpr static int NumberOfChildrenOrTag(BlockType type) {
    switch (type) {
#define RANGE(...)
#define NODE3(F, N, S)            \
  case BlockType::SCOPED_NODE(F): \
    return N;
#include <poincare_junior/src/memory/types.h>
#undef NODE3
#undef RANGE
      default:
        return 0;
    }
  }
};

#undef NARY
#undef NARY2D
#undef GET4TH
#undef NODE1
#undef NODE2
#undef NODE

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

}  // namespace PoincareJ

#endif

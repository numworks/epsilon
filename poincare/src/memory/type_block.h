#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include <omg/code_point.h>
#include <omg/unaligned.h>

#include "block.h"
#include "type_enum.h"
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Poincare::Internal {

/* The items to include in the enum are wrapped with a macro and split in
 * different files to tidy them and be able to use them in different ways. */

/* The macro NODE(name, nbChildren=0, struct additionnalBlocks={}) declares
 * nodes with arity nbChildren (can be NARY) and some optional extra blocks in
 * the header to store special data. */

/* The macro RANGE(name, start, end) creates a named group of previously
 * declared nodes. */

/* Declarations of custom node structs, they are processed only for nodes with 3
 * arguments, ie with a custom node. */

namespace CustomTypeStructs {
/* Declare a struct for each custom node:
 * NODE(RationalNegShort, 0, {
 *   uint8_t absNumerator;
 *   uint8_t denominator;
 * })
 *
 * will produce
 *
 * struct RationalNegShortNode {
 *   uint8_t absNumerator;
 *   uint8_t denominator;
 * };
 */

#define NODE_DECL(F, N, S) struct NODE_NAME(F) S;
#include "types.h"
}  // namespace CustomTypeStructs

consteval bool rangeIsDisabled(uint8_t first, uint8_t last) {
  constexpr const bool isEnabled[] = {
#define NODE_USE(F, N, S) true,
#define UNDEF_NODE_USE(F) false,
#include "types.h"
  };
  for (uint8_t i = first; i <= last; i++) {
    if (isEnabled[i]) {
      return false;
    }
  }
  return true;
}

class TypeBlock : public Block {
 public:
  constexpr TypeBlock(AnyType content) : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(Type::NumberOfTypes));
  }
  constexpr TypeBlock(EnabledType content)
      : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(Type::NumberOfTypes));
  }
  constexpr EnabledType type() const { return static_cast<uint8_t>(m_content); }

  constexpr operator EnabledType() const { return type(); }
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_content); }

#if POINCARE_TREE_LOG
  /* Add an array of names for the Types
   * NODE(MinusOne) => "MinusOne", */
  static constexpr const char* names[] = {
#define NODE_USE(F, N, S) #F,
#define UNDEF_NODE_USE(F) "",
#include "types.h"
  };
#endif

  // Add methods like IsNumber(type) and .isNumber to test range membership
#define RANGE(NAME, FIRST, LAST)                                        \
  static constexpr bool Is##NAME(EnabledType type) {                    \
    constexpr uint8_t firstIndex = static_cast<int>(Type::FIRST) % 256; \
    constexpr uint8_t lastIndex = static_cast<int>(Type::LAST) % 256;   \
    static_assert(firstIndex <= lastIndex);                             \
    if constexpr (rangeIsDisabled(firstIndex, lastIndex)) {             \
      return false;                                                     \
    } else {                                                            \
      return firstIndex <= type && type <= lastIndex;                   \
    }                                                                   \
  }                                                                     \
                                                                        \
  constexpr bool is##NAME() const { return Is##NAME(type()); }

#define UNDEF_RANGE(NAME, FIRST, LAST)                               \
  static constexpr bool Is##NAME(EnabledType type) { return false; } \
  constexpr bool is##NAME() const { return false; }

#define RANGE1(N) RANGE(N, N, N)
#define UNDEF_RANGE1(N) UNDEF_RANGE(N, N, N)

#define NODE_USE(F, N, S) RANGE1(SCOPED_NODE(F))
#define UNDEF_NODE_USE(F) UNDEF_RANGE1(SCOPED_NODE(F))

#include "types.h"
#undef RANGE1
#undef UNDEF_RANGE1

  consteval static size_t DefaultNumberOfMetaBlocks(int N) {
    return N == NARY2D ? 3 : N == NARY ? 2 : N == NARY16 ? 3 : 1;
  }

// Add casts to custom node structs
#define CAST_(F, N, T)                                    \
  CustomTypeStructs::F* to##F() {                         \
    /* assert(type() == static_cast<int>(Type::T)); */    \
    return reinterpret_cast<CustomTypeStructs::F*>(       \
        nextNth(DefaultNumberOfMetaBlocks(N)));           \
  }                                                       \
  const CustomTypeStructs::F* to##F() const {             \
    /* assert(type() == static_cast<int>(Type::T)); */    \
    return reinterpret_cast<const CustomTypeStructs::F*>( \
        nextNth(DefaultNumberOfMetaBlocks(N)));           \
  }
#define CAST(F, N, T) CAST_(F, N, T)
#define NODE_DECL(F, N, S) CAST(NODE_NAME(F), N, SCOPED_NODE(F))
#include "types.h"
#undef CAST_
#undef CAST

  constexpr static bool IsOfType(EnabledType thisType,
                                 std::initializer_list<AnyType> types) {
    for (AnyType t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isOfType(std::initializer_list<AnyType> types) const {
    return IsOfType(type(), types);
  }

  bool isScalarOnly() const {
#if POINCARE_MATRIX
    return !isAMatrixOrContainsMatricesAsChildren();
#else
    return true;
#endif
  }

  // Their next metaBlock contains the numberOfChildren
  constexpr static bool IsNAry(EnabledType type) {
    return NumberOfChildrenOrTag(type) == NARY ||
           NumberOfChildrenOrTag(type) == NARY16;
  }
  constexpr bool isNAry() const { return IsNAry(type()); }
  // NAry with a single metaBlock
  constexpr bool isSimpleNAry() const {
    return isNAry() && nodeSize() == NumberOfMetaBlocks(type());
  }
  constexpr static bool IsNAry16(EnabledType type) {
    return NumberOfChildrenOrTag(type) == NARY16;
  }
  constexpr bool isNAry16() const { return IsNAry16(type()); }

 private:
  constexpr static int NARY = -1;
  constexpr static int NARY2D = -2;
  constexpr static int NARY16 = -3;

 public:
  constexpr static size_t NumberOfMetaBlocks(EnabledType type) {
    switch (type) {
      /* NODE(MinusOne) => DefaultNumberOfMetaBlocks(0) + 0
       * NODE(Mult, NARY) => DefaultNumberOfMetaBlocks(NARY) + 0
       * NODE(IntegerNegShort, 0, { uint8_t absValue; }) =>
       *   DefaultNumberOfMetaBlocks(0) + sizeof(IntegerNegShortNode)
       */
#define NODE_USE(F, N, S)    \
  case Type::SCOPED_NODE(F): \
    return DefaultNumberOfMetaBlocks(N) + S;
#include "types.h"
      default:
        return 1;
    }
  }

  constexpr size_t nodeSize() const {
    EnabledType t = type();
    size_t numberOfMetaBlocks = NumberOfMetaBlocks(t);
    // NOTE: Make sure new Types are handled here.
    switch (t) {
      case Type::IntegerPosBig:
      case Type::IntegerNegBig: {
        uint8_t numberOfDigits = static_cast<uint8_t>(*next());
        return numberOfMetaBlocks + numberOfDigits;
      }
      case Type::RationalPosBig:
      case Type::RationalNegBig: {
        uint8_t numberOfDigitsNumerator = static_cast<uint8_t>(*next());
        uint8_t numberOfDigitsDenominator = static_cast<uint8_t>(*nextNth(2));
        return numberOfMetaBlocks +
               static_cast<size_t>(numberOfDigitsNumerator) +
               static_cast<size_t>(numberOfDigitsDenominator);
      }
      case Type::Polynomial: {
        uint8_t numberOfTerms = static_cast<uint8_t>(*next()) - 1;
        return numberOfMetaBlocks + numberOfTerms;
      }
      case Type::UserSymbol: {
        uint8_t numberOfChars = static_cast<uint8_t>(*nextNth(1));
        return numberOfMetaBlocks + numberOfChars;
      }
      case Type::UserFunction:
#if POINCARE_SEQUENCE
      case Type::UserSequence:
#endif
      {
        uint8_t numberOfChars = static_cast<uint8_t>(*next());
        return numberOfMetaBlocks + numberOfChars;
      }
      case Type::Arbitrary: {
        uint16_t size = static_cast<uint8_t>(*nextNth(3)) << 8 |
                        static_cast<uint8_t>(*nextNth(2));
        return numberOfMetaBlocks + size;
      }
      default:
        return numberOfMetaBlocks;
    }
  }

  constexpr int numberOfChildren() const {
    int n = NumberOfChildrenOrTag(type());
    if (n >= 0) {
      return n;
    } else if (n == NARY) {
      return static_cast<uint8_t>(*next());
    } else if (n == NARY16) {
      return OMG::unalignedShort(next());
    } else {
      assert(n == NARY2D);
      return static_cast<uint8_t>(*next()) * static_cast<uint8_t>(*nextNth(2));
    }
  }

  constexpr static int NumberOfChildren(EnabledType type) {
    assert(NumberOfChildrenOrTag(type) != NARY &&
           NumberOfChildrenOrTag(type) != NARY2D);
    return NumberOfChildrenOrTag(type);
  }

 private:
  constexpr static int NumberOfChildrenOrTag(EnabledType type) {
    switch (type) {
      /* NODE(MinusOne) => 0
       * NODE(Pow, 2) => 2 */
#define NODE_USE(F, N, S)    \
  case Type::SCOPED_NODE(F): \
    return N;
#include "types.h"
      default:
        return 0;
    }
  }
};  // namespace Poincare::Internal

static_assert(sizeof(TypeBlock) == sizeof(Block));

}  // namespace Poincare::Internal

#endif

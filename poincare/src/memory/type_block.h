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
#include "types.inc"
}  // namespace CustomTypeStructs

consteval bool rangeIsDisabled(uint8_t first, uint8_t last) {
  constexpr const bool isEnabled[] = {
#define NODE_USE(F, N, S) true,
#define DISABLED_NODE_USE(F, N, S) false,
#include "types.inc"
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
  constexpr TypeBlock(Type content) : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(Type::NumberOfTypes));
  }
  constexpr Type type() const { return static_cast<uint8_t>(m_content); }

  constexpr operator Type() const { return type(); }
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_content); }

#if POINCARE_TREE_LOG
  /* Add an array of names for the Types
   * NODE(MinusOne) => "MinusOne", */
  constexpr static const char* names[] = {
#define NODE_USE(F, N, S) #F,
#define DISABLED_NODE_USE(F, N, S) "",
#include "types.inc"
  };
#endif

  // Add methods like IsNumber(type) and .isNumber to test range membership
#define RANGE(NAME, FIRST, LAST)                                             \
  __attribute__((always_inline)) constexpr static bool Is##NAME(Type type) { \
    constexpr uint8_t firstIndex = static_cast<uint8_t>(Type::FIRST);        \
    constexpr uint8_t lastIndex = static_cast<uint8_t>(Type::LAST);          \
    static_assert(firstIndex <= lastIndex);                                  \
    if constexpr (rangeIsDisabled(firstIndex, lastIndex)) {                  \
      return false;                                                          \
    } else {                                                                 \
      return firstIndex <= type && type <= lastIndex;                        \
    }                                                                        \
  }                                                                          \
                                                                             \
  __attribute__((always_inline)) constexpr bool is##NAME() const {           \
    return Is##NAME(type());                                                 \
  }

#define RANGE1(N) RANGE(N, N, N)
#define NODE_USE(F, N, S) RANGE1(SCOPED_NODE(F))
#include "types.inc"
#undef RANGE1

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
#include "types.inc"
#undef CAST_
#undef CAST

  /* IsOfType takes AnyType and not Type because we want to allow some
   * disabled types in an IsOfType list. */
  constexpr static bool IsOfType(Type thisType,
                                 std::initializer_list<AnyType> types) {
    for (AnyType t : types) {
      if (t.isEnabled() && thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isOfType(std::initializer_list<AnyType> types) const {
    return IsOfType(type(), types);
  }

  bool isScalarOnly() const {
    return !(POINCARE_MATRIX && isAMatrixOrContainsMatricesAsChildren());
  }

  static bool ProducesList(Type type) {
    return IsOfType(type, {Type::List, Type::ListSequence, Type::RandIntNoRep});
  }

  // Their next metaBlock contains the numberOfChildren
  constexpr static bool IsNAry(Type type) {
    return NumberOfChildrenOrTag(type) == NARY ||
           NumberOfChildrenOrTag(type) == NARY16;
  }
  constexpr bool isNAry() const { return IsNAry(type()); }
  // NAry with a single metaBlock
  constexpr bool isSimpleNAry() const {
    return isNAry() && nodeSize() == NumberOfMetaBlocks(type());
  }
  constexpr static bool IsNAry16(Type type) {
    return NumberOfChildrenOrTag(type) == NARY16;
  }
  constexpr bool isNAry16() const { return IsNAry16(type()); }

 private:
  constexpr static int NARY = -1;
  constexpr static int NARY2D = -2;
  constexpr static int NARY16 = -3;

 public:
  constexpr static size_t NumberOfMetaBlocks(Type type) {
    /* NOTE: Make sure to also update
     * [NumberOfChidrenOrTagAndNumberOfMetaBlocks] */
    switch (type) {
      /* NODE(MinusOne) => DefaultNumberOfMetaBlocks(0) + 0
       * NODE(Mult, NARY) => DefaultNumberOfMetaBlocks(NARY) + 0
       * NODE(IntegerNegShort, 0, { uint8_t absValue; }) =>
       *   DefaultNumberOfMetaBlocks(0) + sizeof(IntegerNegShortNode)
       */
#define NODE_USE(F, N, S)    \
  case Type::SCOPED_NODE(F): \
    return DefaultNumberOfMetaBlocks(N) + S;
#define DISABLED_NODE_USE(F, N, S)
#include "types.inc"
      default:
        return 1;
    }
  }

  struct NodeInfo {
    int numberOfChildren;
    size_t nodeSize;
  };

  inline constexpr NodeInfo numberOfChildrenAndNodeSize() const {
    Type t = type();
    NodeInfo ni = NumberOfChidrenOrTagAndNumberOfMetaBlocks(t);
    /* ni.nodeSize here is actually numberOfMetaBlocks.
     * ni.numberOfChildren here is actually numberOfChildrenOrTag */
    return {numberOfChildren(ni.numberOfChildren), nodeSize(t, ni.nodeSize)};
  }

  constexpr size_t nodeSize() const {
    Type t = type();
    return nodeSize(t, NumberOfMetaBlocks(t));
  }

  constexpr int numberOfChildren() const {
    return numberOfChildren(NumberOfChildrenOrTag(type()));
  }

  constexpr static int NumberOfChildren(Type type) {
    assert(NumberOfChildrenOrTag(type) != NARY &&
           NumberOfChildrenOrTag(type) != NARY2D);
    return NumberOfChildrenOrTag(type);
  }

 private:
  inline constexpr size_t nodeSize(Type t, size_t numberOfMetaBlocks) const {
    switch (t) {
      case Type::UserSymbol:
      case Type::UserFunction:
#if POINCARE_SEQUENCE
      case Type::UserSequence:
#endif
      case Type::Polynomial:
      case Type::RationalPosBig:
      case Type::IntegerPosBig:
      case Type::RationalNegBig:
      case Type::IntegerNegBig: {
        numberOfMetaBlocks += static_cast<uint8_t>(*next());
        if (t == Type::RationalPosBig || t == Type::RationalNegBig) {
          numberOfMetaBlocks += static_cast<uint8_t>(*nextNth(2));
        } else if (t == Type::Polynomial) {
          --numberOfMetaBlocks;
        }
        return numberOfMetaBlocks;
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

  inline constexpr int numberOfChildren(int numberOfChildrenOrTag) const {
    if (numberOfChildrenOrTag >= 0) {
      return numberOfChildrenOrTag;
    } else if (numberOfChildrenOrTag == NARY) {
      return static_cast<uint8_t>(*next());
    } else if (numberOfChildrenOrTag == NARY16) {
      return OMG::unalignedShort(next());
    } else {
      assert(numberOfChildrenOrTag == NARY2D);
      return static_cast<uint8_t>(*next()) * static_cast<uint8_t>(*nextNth(2));
    }
  }

  constexpr static int NumberOfChildrenOrTag(Type type) {
    /* NOTE: Make sure to also update
     * [NumberOfChidrenOrTagAndNumberOfMetaBlocks] */
    switch (type) {
      /* NODE(MinusOne) => 0
       * NODE(Pow, 2) => 2 */
#define NODE_USE(F, N, S)    \
  case Type::SCOPED_NODE(F): \
    return N;
#define DISABLED_NODE_USE(F, N, S)
#include "types.inc"
      default:
        return 0;
    }
  }

  constexpr static NodeInfo NumberOfChidrenOrTagAndNumberOfMetaBlocks(
      /* NOTE: Make sure to also update [NumberOfChidrenOrTag] and
       * [NumberOfMetaBlocks] */
      Type type) {
    switch (type) {
      /* NODE(MinusOne) => {0, DefaultNumberOfMetaBlocks(0) + 0}
       * NODE(Mult, NARY) => {NARY, DefaultNumberOfMetaBlocks(NARY) + 0}
       * NODE(IntegerNegShort, 0, { uint8_t absValue; }) =>
       *   {0, DefaultNumberOfMetaBlocks(0) + sizeof(IntegerNegShortNode)} */
#define NODE_USE(F, N, S)    \
  case Type::SCOPED_NODE(F): \
    return {N, DefaultNumberOfMetaBlocks(N) + S};
#define DISABLED_NODE_USE(F, N, S)
#include "types.inc"
      default:
        return {0, 1};
    }
  }
};

static_assert(sizeof(TypeBlock) == sizeof(Block));

}  // namespace Poincare::Internal

#endif

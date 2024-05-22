#ifndef POINCARE_MEMORY_TYPE_BLOCK_H
#define POINCARE_MEMORY_TYPE_BLOCK_H

#include <omg/code_point.h>

#include "block.h"

namespace Poincare::Internal {

/* The items to include in the enum are wrapped with a macro and split in
 * different files to tidy them and be able to use them in different ways. */

/* The macro NODE(name, nbChildren=0, struct additionnalBlocks={}) declares
 * nodes with arity nbChildren (can be NARY) and some optional extra blocks in
 * the header to store special data. */

/* The macro RANGE(name, start, end) creates a named group of previously
 * declared nodes. */

// Helper to return struct names such as AbsLayoutNode
#define NODE_NAME__(F) F##Node
#define NODE_NAME_(F) NODE_NAME__(F)
#define NODE_NAME(F) NODE_NAME_(SCOPED_NODE(F))

/* Boilerplate to alias NODE(F) as NODE3(F, 0, 0), NODE3 is the varying macro.
 * The first VA_ARGS will push the other arguments of GET4TH allowing it to
 * select the suitable NODE_X macro and call it with the second VA_ARGS list.
 * With 2 args, we have GET4TH(A, B, NODE3, NODE2)(A, B) => NODE2(A, B) */
#define GET4TH(A, B, C, D, ...) D
#define NODE1(F) NODE_USE(F, 0, 0)
#define NODE2(F, N) NODE_USE(F, N, 0)
#define NODE3(F, N, S) NODE_DECL(F, S) NODE_USE(F, N, sizeof(NODE_NAME(F)))
#define NODE(...) GET4TH(__VA_ARGS__, NODE3, NODE2, NODE1)(__VA_ARGS__)

/* Declarations of custom node structs, they are processed only for nodes with 3
 * arguments, ie with a custom node. */

/* For instance, the declaration
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

#define NODE_DECL(F, S) struct NODE_NAME(F) S;
#include "types.h"

enum class Type : uint8_t {
/* Add all the types to the enum
 * NODE(MinusOne) => MinusOne,
 * NODE(Fraction) in layout.h => FractionLayout,
 */
#define NODE_USE(F, N, S) SCOPED_NODE(F),
#include "types.h"
};

enum class LayoutType : uint8_t {
/* Members of LayoutType have the same values as their Type counterpart
 * NODE(Fraction) => Fraction = Type::FractionLayout,
 */
#define RANGE(...)
#define NODE_DECL(...)
#define NODE_USE(F, N, S) F = static_cast<uint8_t>(Type::F##Layout),
#include <poincare/src/layout/types.h>
#undef NODE_DECL
#undef NODE_USE
#undef RANGE
};

class TypeBlock : public Block {
 public:
  constexpr TypeBlock(Type content) : Block(static_cast<uint8_t>(content)) {
    assert(m_content < static_cast<uint8_t>(Type::NumberOfTypes));
  }
  constexpr Type type() const { return static_cast<Type>(m_content); }

  bool operator==(const TypeBlock& other) const = default;
  constexpr bool operator==(Type t) const { return type() == t; }
  bool operator!=(const TypeBlock& other) const = default;
  constexpr bool operator!=(Type t) const { return type() != t; }
  constexpr operator Type() const { return type(); }

#if POINCARE_TREE_LOG
  /* Add an array of names for the Types
   * NODE(MinusOne) => "MinusOne", */
  static constexpr const char* names[] = {
#define NODE_USE(F, N, S) #F,
#include "types.h"
  };
#endif

  // Add methods like IsNumber(type) and .isNumber to test range membership
#define RANGE(NAME, FIRST, LAST)                      \
  static constexpr bool Is##NAME(Type type) {         \
    static_assert(Type::FIRST <= Type::LAST);         \
    return Type::FIRST <= type && type <= Type::LAST; \
  }                                                   \
                                                      \
  constexpr bool is##NAME() const { return Is##NAME(type()); }

#define RANGE1(N) RANGE(N, N, N)

#define NODE_USE(F, N, S) RANGE1(SCOPED_NODE(F))

#include "types.h"
#undef RANGE1

  // Add casts to custom node structs
#define CAST_(F, T)                            \
  F* to##F() {                                 \
    assert(type() == Type::T);                 \
    return reinterpret_cast<F*>(next());       \
  }                                            \
  const F* to##F() const {                     \
    assert(type() == Type::T);                 \
    return reinterpret_cast<const F*>(next()); \
  }
#define CAST(F, T) CAST_(F, T)
#define NODE_DECL(F, S) CAST(NODE_NAME(F), SCOPED_NODE(F))
#include "types.h"
#undef CAST_
#undef CAST

  constexpr static bool IsOfType(Type thisType,
                                 std::initializer_list<Type> types) {
    for (Type t : types) {
      if (thisType == t) {
        return true;
      }
    }
    return false;
  }

  constexpr bool isOfType(std::initializer_list<Type> types) const {
    return IsOfType(type(), types);
  }

  bool isScalarOnly() const { return !isAMatrixOrContainsMatricesAsChildren(); }

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

  consteval static size_t DefaultNumberOfMetaBlocks(int N) {
    return N == NARY2D ? 3 : N == NARY ? 2 : N == NARY16 ? 3 : 1;
  }

 public:
  constexpr static size_t NumberOfMetaBlocks(Type type) {
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
    Type t = type();
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
        uint8_t numberOfChars = static_cast<uint8_t>(*nextNth(2));
        return numberOfMetaBlocks + numberOfChars;
      }
      case Type::UserFunction:
      case Type::UserSequence: {
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
    } else if (n == NARY) {
      return static_cast<uint8_t>(*next());
    } else if (n == NARY16) {
      return *reinterpret_cast<const uint16_t*>(next());
    } else {
      assert(n == NARY2D);
      return static_cast<uint8_t>(*next()) * static_cast<uint8_t>(*nextNth(2));
    }
  }

  constexpr static int NumberOfChildren(Type type) {
    assert(NumberOfChildrenOrTag(type) != NARY &&
           NumberOfChildrenOrTag(type) != NARY2D);
    return NumberOfChildrenOrTag(type);
  }

 private:
  constexpr static int NumberOfChildrenOrTag(Type type) {
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
};

#undef GET4TH
#undef NODE1
#undef NODE2
#undef NODE3
#undef NODE

static_assert(sizeof(TypeBlock) == sizeof(Block));

// Add a TreeBorder blocks at the end to assert we don't navigate out of it.
template <int size>
class BlockBuffer {
 public:
  constexpr BlockBuffer() {
#if ASSERTIONS
    m_blocks[size] = Type::TreeBorder;
#endif
  }
  constexpr TypeBlock* blocks() {
    return static_cast<TypeBlock*>(static_cast<Block*>(m_blocks));
  }
  consteval const TypeBlock* blocks() const {
    return static_cast<const TypeBlock*>(m_blocks);
  }

 private:
#if ASSERTIONS
  Block m_blocks[size + 1];
#else
  Block m_blocks[size];
#endif
};

}  // namespace Poincare::Internal

#endif

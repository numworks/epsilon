#ifndef POINCARE_EXPRESSION_K_TREE_H
#define POINCARE_EXPRESSION_K_TREE_H

#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/k_tree.h>

#include <bit>

namespace PoincareJ {

// Constructors

constexpr auto KAbs = KUnary<BlockType::Abs>();
constexpr auto KCos = KUnary<BlockType::Cosine>();
constexpr auto KSin = KUnary<BlockType::Sine>();
constexpr auto KTan = KUnary<BlockType::Tangent>();
constexpr auto KACos = KUnary<BlockType::ArcCosine>();
constexpr auto KASin = KUnary<BlockType::ArcSine>();
constexpr auto KATan = KUnary<BlockType::ArcTangent>();
constexpr auto KLog = KUnary<BlockType::Log>();
constexpr auto KLn = KUnary<BlockType::Ln>();
constexpr auto KExp = KUnary<BlockType::Exponential>();
constexpr auto KFact = KUnary<BlockType::Factorial>();
constexpr auto KSqrt = KUnary<BlockType::SquareRoot>();

constexpr auto KLogarithm = KBinary<BlockType::Logarithm>();
constexpr auto KTrig = KBinary<BlockType::Trig>();
constexpr auto KTrigDiff = KBinary<BlockType::TrigDiff>();
constexpr auto KDiv = KBinary<BlockType::Division>();
constexpr auto KSub = KBinary<BlockType::Subtraction>();
constexpr auto KPow = KBinary<BlockType::Power>();
constexpr auto KPowReal = KBinary<BlockType::PowerReal>();
constexpr auto KDiff = KTrinary<BlockType::Derivative>();

constexpr auto KAdd = KNAry<BlockType::Addition>();
constexpr auto KMult = KNAry<BlockType::Multiplication>();
constexpr auto KSet = KNAry<BlockType::Set>();
constexpr auto KSystemList = KNAry<BlockType::SystemList>();

/* if you want to add operator+ and so on, you can revert them from the commit
 * [poincare_junior] Split tree_constructor.h */

// Alias only for readability
template <uint8_t... Values>
using Exponents = KTree<Values...>;

/* The first function is responsible of building the actual representation from
 * Trees while the other one is just here to allow the function to take
 * TreeCompatible arguments like integer litterals. */

template <TreeConcept Exp, TreeConcept... CTS>
static consteval auto __Pol(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  return Concat<KTree<BlockType::Polynomial, Size>, Exp, CTS...>();
}

template <TreeConcept Exp, TreeCompatibleConcept... CTS>
static consteval auto KPol(Exp exponents, CTS... args) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(
      Exp::k_size == Size - 1,
      "Number of children and exponents do not match in constant polynomial");
  return __Pol(exponents, KTree(args)...);
}

/* Immediates are used to represent numerical constants of the code (like 2_e)
 * temporarily before they are cast to Trees, this allows writing -2_e. */

template <int V>
class IntegerLitteral : public AbstractTreeCompatible {
 public:
  // Once a deduction guide has chosen the KTree for the litteral, build it
  template <Block... B>
  consteval operator KTree<B...>() {
    return KTree<B...>();
  }

  constexpr operator const Tree*() const { return KTree(IntegerLitteral<V>()); }
  const Tree* operator->() const { return KTree(IntegerLitteral<V>()); }

  consteval IntegerLitteral<-V> operator-() { return IntegerLitteral<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

// template <int8_t V> using Inti = Tree<BlockType::IntegerShort, V,
// BlockType::IntegerShort>; template <int8_t V> Tree(Immediate<V>)->Inti<V>; //
// only GCC accepts this one

// Deduction guides to create the smallest Tree that can represent the Immediate

KTree(IntegerLitteral<-1>)->KTree<BlockType::MinusOne>;
KTree(IntegerLitteral<0>)->KTree<BlockType::Zero>;
KTree(IntegerLitteral<1>)->KTree<BlockType::One>;
KTree(IntegerLitteral<2>)->KTree<BlockType::Two>;

template <int V>
  requires(V >= INT8_MIN && V <= INT8_MAX)
KTree(IntegerLitteral<V>) -> KTree<BlockType::IntegerShort, V>;

template <int V>
  requires(V > INT8_MAX && Integer::NumberOfDigits(V) == 1)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerPosBig, 1, Bit::getByteAtIndex(V, 0)>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 2)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerPosBig, 2, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1)>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 3)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerPosBig, 3, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2)>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 4)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerPosBig, 4, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
             Bit::getByteAtIndex(V, 3)>;

template <int V>
  requires(V < INT8_MIN && Integer::NumberOfDigits(-V) == 1)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerNegBig, 1, Bit::getByteAtIndex(-V, 0)>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 2)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerNegBig, 2, Bit::getByteAtIndex(-V, 0),
             Bit::getByteAtIndex(-V, 1)>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 3)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerNegBig, 3, Bit::getByteAtIndex(-V, 0),
             Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2)>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 4)
KTree(IntegerLitteral<V>)
    -> KTree<BlockType::IntegerNegBig, 4, Bit::getByteAtIndex(-V, 0),
             Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2),
             Bit::getByteAtIndex(-V, 3)>;

// TODO new node_constructor
constexpr KTree KUndef = KTree<BlockType::Undefined>();
constexpr KTree KHalf = KTree<BlockType::Half>();

constexpr KTree π_e =
    KTree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::Pi)>();

constexpr KTree e_e =
    KTree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::E)>();

// TODO: move in OMG?
constexpr static uint64_t IntegerValue(const char* str, size_t size) {
  uint64_t value = 0;
  for (size_t i = 0; i < size - 1; i++) {
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    assert(value <= (UINT64_MAX - digit) / 10);
    value = 10 * value + digit;
  }
  return value;
}

constexpr bool HasDecimalPoint(const char* str, size_t size) {
  for (size_t i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      return true;
    }
  }
  return false;
}

constexpr static float FloatValue(const char* str, size_t size) {
  float value = 0;
  bool fractionalPart = false;
  float base = 1;
  for (size_t i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      fractionalPart = true;
      continue;
    }
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    assert(value <= (UINT64_MAX - digit) / 10);
    if (!fractionalPart) {
      value = 10 * value + digit;
    } else {
      // TODO use a better algo precision-wise
      base *= 10.f;
      value += digit / base;
    }
  }
  return value;
}

/* A template <float V> would be cool but support for this is poor yet so we
 * have to store the bit representation of the float. */
template <uint32_t V>
class FloatLitteral : public AbstractTreeCompatible {
 public:
  template <Block... B>
  consteval operator KTree<B...>() {
    return KTree<B...>();
  }

  constexpr operator const Tree*() const { return KTree(FloatLitteral<V>()); }
  const Tree* operator->() const { return KTree(FloatLitteral<V>()); }

  // Since we are using the representation we have to manually flip the sign bit
  consteval auto operator-() { return FloatLitteral<V ^ (1 << 31)>(); }
};

template <uint32_t V>
KTree(FloatLitteral<V>)
    -> KTree<BlockType::Float, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
             Bit::getByteAtIndex(V, 3)>;

template <char... C>
consteval auto operator"" _e() {
  constexpr const char value[] = {C..., '\0'};
  if constexpr (HasDecimalPoint(value, sizeof...(C) + 1)) {
    return FloatLitteral<std::bit_cast<uint32_t>(
        FloatValue(value, sizeof...(C) + 1))>();
  } else {
    return IntegerLitteral<IntegerValue(value, sizeof...(C) + 1)>();
  }
}

// specialized from
// https://stackoverflow.com/questions/60434033/how-do-i-expand-a-compile-time-stdarray-into-a-parameter-pack/60440611#60440611

template <String S,
          typename IS = decltype(std::make_index_sequence<S.size() - 1>())>
struct Variable;

template <String S, std::size_t... I>
struct Variable<S, std::index_sequence<I...>> {
  static_assert(!OMG::Print::IsDigit(S[0]),
                "Integer litterals should be written without quotes");
  using tree = KTree<BlockType::UserSymbol, sizeof...(I), S[I]...>;
};

template <String S>
consteval auto operator"" _e() {
  return typename Variable<S>::tree();
}

}  // namespace PoincareJ

#endif

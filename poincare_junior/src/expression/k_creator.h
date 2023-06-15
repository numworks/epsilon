#ifndef POINCARE_EXPRESSION_K_CREATOR_H
#define POINCARE_EXPRESSION_K_CREATOR_H

#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/k_creator.h>

#include <bit>

namespace PoincareJ {

// Constructors

template <class... Args>
consteval auto KAbs(Args... args) {
  return KUnary<BlockType::Abs>(args...);
}

template <class... Args>
consteval auto KCos(Args... args) {
  return KUnary<BlockType::Cosine>(args...);
}

template <class... Args>
consteval auto KSin(Args... args) {
  return KUnary<BlockType::Sine>(args...);
}

template <class... Args>
consteval auto KTan(Args... args) {
  return KUnary<BlockType::Tangent>(args...);
}

template <class... Args>
consteval auto KACos(Args... args) {
  return KUnary<BlockType::ArcCosine>(args...);
}

template <class... Args>
consteval auto KASin(Args... args) {
  return KUnary<BlockType::ArcSine>(args...);
}

template <class... Args>
consteval auto KATan(Args... args) {
  return KUnary<BlockType::ArcTangent>(args...);
}

template <class... Args>
consteval auto KLog(Args... args) {
  return KUnary<BlockType::Log>(args...);
}

template <class... Args>
consteval auto KLn(Args... args) {
  return KUnary<BlockType::Ln>(args...);
}

template <class... Args>
consteval auto KExp(Args... args) {
  return KUnary<BlockType::Exponential>(args...);
}

template <class... Args>
consteval auto KFact(Args... args) {
  return KUnary<BlockType::Factorial>(args...);
}

template <class... Args>
consteval auto KLogarithm(Args... args) {
  return KBinary<BlockType::Logarithm>(args...);
}

template <class... Args>
consteval auto KTrig(Args... args) {
  return KBinary<BlockType::Trig>(args...);
}

template <class... Args>
consteval auto KTrigDiff(Args... args) {
  return KBinary<BlockType::TrigDiff>(args...);
}

template <class... Args>
consteval auto KDiv(Args... args) {
  return KBinary<BlockType::Division>(args...);
}

template <class... Args>
consteval auto KSub(Args... args) {
  return KBinary<BlockType::Subtraction>(args...);
}

template <class... Args>
consteval auto KPow(Args... args) {
  return KBinary<BlockType::Power>(args...);
}

template <class... Args>
consteval auto KDiff(Args... args) {
  return KTrinary<BlockType::Derivative>(args...);
}

template <class... Args>
consteval auto KAdd(Args... args) {
  return KNAry<BlockType::Addition>(args...);
}

template <class... Args>
consteval auto KMult(Args... args) {
  return KNAry<BlockType::Multiplication>(args...);
}

template <class... Args>
consteval auto KSet(Args... args) {
  return KNAry<BlockType::Set>(args...);
}

template <class... Args>
consteval auto KSystemList(Args... args) {
  return KNAry<BlockType::SystemList>(args...);
}

/* if you want to add operator+ and so on, you can revert them from the commit
 * [poincare_junior] Split tree_constructor.h */

// Alias only for readability
template <uint8_t... Values>
using Exponents = Tree<Values...>;

/* The first function is responsible of building the actual representation from
 * Trees while the other one is just here to allow the function to take
 * TreeCompatible arguments like integer litterals. */

template <TreeConcept Exp, TreeConcept... CTS>
static consteval auto __Pol(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  return Concat<Tree<BlockType::Polynomial, Size>, Exp,
                Tree<Size, BlockType::Polynomial>, CTS...>();
}

template <TreeConcept Exp, TreeCompatibleConcept... CTS>
static consteval auto KPol(Exp exponents, CTS... args) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(
      Exp::k_size == Size - 1,
      "Number of children and exponents do not match in constant polynomial");
  return __Pol(exponents, Tree(args)...);
}

/* Immediates are used to represent numerical constants of the code (like 2_e)
 * temporarily before they are cast to Trees, this allows writing -2_e. */

template <int V>
class IntegerLitteral : public AbstractTreeCompatible {
 public:
  // once a deduction guide as required a given Tree from the immediate, build
  // it
  template <Block... B>
  consteval operator Tree<B...>() {
    return Tree<B...>();
  }

  constexpr operator const Node() { return Tree(IntegerLitteral<V>()); }

  consteval IntegerLitteral<-V> operator-() { return IntegerLitteral<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

// template <int8_t V> using Inti = Tree<BlockType::IntegerShort, V,
// BlockType::IntegerShort>; template <int8_t V> Tree(Immediate<V>)->Inti<V>; //
// only GCC accepts this one

// Deduction guides to create the smallest Tree that can represent the Immediate

Tree(IntegerLitteral<-1>)->Tree<BlockType::MinusOne>;
Tree(IntegerLitteral<0>)->Tree<BlockType::Zero>;
Tree(IntegerLitteral<1>)->Tree<BlockType::One>;
Tree(IntegerLitteral<2>)->Tree<BlockType::Two>;

template <int V>
  requires(V >= INT8_MIN && V <= INT8_MAX)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerShort, V, BlockType::IntegerShort>;

template <int V>
  requires(V > INT8_MAX && Integer::NumberOfDigits(V) == 1)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerPosBig, 1, Bit::getByteAtIndex(V, 0), 1,
            BlockType::IntegerPosBig>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 2)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerPosBig, 2, Bit::getByteAtIndex(V, 0),
            Bit::getByteAtIndex(V, 1), 2, BlockType::IntegerPosBig>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 3)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerPosBig, 3, Bit::getByteAtIndex(V, 0),
            Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2), 3,
            BlockType::IntegerPosBig>;

template <int V>
  requires(V > 0 && Integer::NumberOfDigits(V) == 4)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerPosBig, 4, Bit::getByteAtIndex(V, 0),
            Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
            Bit::getByteAtIndex(V, 3), 4, BlockType::IntegerPosBig>;

template <int V>
  requires(V < INT8_MIN && Integer::NumberOfDigits(-V) == 1)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerNegBig, 1, Bit::getByteAtIndex(-V, 0), 1,
            BlockType::IntegerNegBig>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 2)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerNegBig, 2, Bit::getByteAtIndex(-V, 0),
            Bit::getByteAtIndex(-V, 1), 2, BlockType::IntegerNegBig>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 3)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerNegBig, 3, Bit::getByteAtIndex(-V, 0),
            Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2), 3,
            BlockType::IntegerNegBig>;

template <int V>
  requires(V < 0 && Integer::NumberOfDigits(-V) == 4)
Tree(IntegerLitteral<V>)
    -> Tree<BlockType::IntegerNegBig, 4, Bit::getByteAtIndex(-V, 0),
            Bit::getByteAtIndex(-V, 1), Bit::getByteAtIndex(-V, 2),
            Bit::getByteAtIndex(-V, 3), 4, BlockType::IntegerNegBig>;

// TODO new node_constructor
constexpr Tree KUndef = Tree<BlockType::Undefined>();

constexpr Tree π_e =
    Tree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::Pi),
         BlockType::Constant>();

constexpr Tree e_e =
    Tree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::E),
         BlockType::Constant>();

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
  consteval operator Tree<B...>() {
    return Tree<B...>();
  }

  constexpr operator const Node() { return Tree(FloatLitteral<V>()); }

  // Since we are using the representation we have to manually flip the sign bit
  consteval auto operator-() { return FloatLitteral<V ^ (1 << 31)>(); }
};

template <uint32_t V>
Tree(FloatLitteral<V>)
    -> Tree<BlockType::Float, Bit::getByteAtIndex(V, 0),
            Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
            Bit::getByteAtIndex(V, 3), BlockType::Float>;

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
  using tree = Tree<BlockType::UserSymbol, sizeof...(I), S[I]..., sizeof...(I),
                    BlockType::UserSymbol>;
};

template <String S>
consteval auto operator"" _e() {
  return typename Variable<S>::tree();
}

}  // namespace PoincareJ

#endif

#ifndef POINCARE_EXPRESSION_K_TREE_H
#define POINCARE_EXPRESSION_K_TREE_H

#include <omg/arithmetic.h>
#include <poincare/src/memory/k_tree.h>

#include <bit>

#include "parametric.h"
#include "sign.h"

namespace Poincare::Internal {
namespace KTrees {

// Constructors

constexpr auto KUndef = KTree<Type::Undef>();
constexpr auto KNonReal = KTree<Type::NonReal>();
constexpr auto KInf = KTree<Type::Inf>();

constexpr auto π_e = KTree<Type::Pi>();
constexpr auto e_e = KTree<Type::EulerE>();
constexpr auto i_e = KTree<Type::ComplexI>();

constexpr auto KAbs = KUnary<Type::Abs>();
constexpr auto KCos = KUnary<Type::Cos>();
constexpr auto KSin = KUnary<Type::Sin>();
constexpr auto KTan = KUnary<Type::Tan>();
constexpr auto KTanRad = KUnary<Type::TanRad>();
constexpr auto KACos = KUnary<Type::ACos>();
constexpr auto KASin = KUnary<Type::ASin>();
constexpr auto KATan = KUnary<Type::ATan>();
constexpr auto KATanRad = KUnary<Type::ATanRad>();
constexpr auto KLog = KUnary<Type::Log>();
constexpr auto KLn = KUnary<Type::Ln>();
constexpr auto KLnReal = KUnary<Type::LnReal>();
constexpr auto KExp = KUnary<Type::Exp>();
constexpr auto KFact = KUnary<Type::Fact>();
constexpr auto KSqrt = KUnary<Type::Sqrt>();
constexpr auto KRe = KUnary<Type::Re>();
constexpr auto KIm = KUnary<Type::Im>();
constexpr auto KArg = KUnary<Type::Arg>();
constexpr auto KConj = KUnary<Type::Conj>();
constexpr auto KOpposite = KUnary<Type::Opposite>();
constexpr auto KFloor = KUnary<Type::Floor>();
constexpr auto KCeil = KUnary<Type::Ceil>();
constexpr auto KFrac = KUnary<Type::Frac>();
constexpr auto KListSum = KUnary<Type::ListSum>();
constexpr auto KMin = KUnary<Type::Min>();
constexpr auto KMax = KUnary<Type::Max>();
constexpr auto KSec = KUnary<Type::Sec>();
constexpr auto KCsc = KUnary<Type::Csc>();
constexpr auto KCot = KUnary<Type::Cot>();
constexpr auto KASec = KUnary<Type::ASec>();
constexpr auto KACsc = KUnary<Type::ACsc>();
constexpr auto KACot = KUnary<Type::ACot>();
constexpr auto KCosH = KUnary<Type::CosH>();
constexpr auto KSinH = KUnary<Type::SinH>();
constexpr auto KTanH = KUnary<Type::TanH>();
constexpr auto KArCosH = KUnary<Type::ArCosH>();
constexpr auto KArSinH = KUnary<Type::ArSinH>();
constexpr auto KArTanH = KUnary<Type::ArTanH>();
constexpr auto KPercentSimple = KUnary<Type::PercentSimple>();
constexpr auto KParenthesis = KUnary<Type::Parenthesis>();

constexpr auto KATrig = KBinary<Type::ATrig>();
constexpr auto KLogarithm = KBinary<Type::Logarithm>();
constexpr auto KTrig = KBinary<Type::Trig>();
constexpr auto KTrigDiff = KBinary<Type::TrigDiff>();
constexpr auto KDiv = KBinary<Type::Div>();
constexpr auto KSub = KBinary<Type::Sub>();
constexpr auto KPow = KBinary<Type::Pow>();
constexpr auto KPowReal = KBinary<Type::PowReal>();
constexpr auto KPowMatrix = KBinary<Type::PowMatrix>();
constexpr auto KDep = KBinary<Type::Dependency>();
constexpr auto KRound = KBinary<Type::Round>();
constexpr auto KListElement = KBinary<Type::ListElement>();
constexpr auto KMean = KBinary<Type::Mean>();
constexpr auto KBinomial = KBinary<Type::Binomial>();
constexpr auto KPermute = KBinary<Type::Permute>();
constexpr auto KRoot = KBinary<Type::Root>();
constexpr auto KPercentAddition = KBinary<Type::PercentAddition>();
constexpr auto KMixedFraction = KBinary<Type::MixedFraction>();
constexpr auto KPoint = KBinary<Type::Point>();
constexpr auto KStore = KBinary<Type::Store>();
constexpr auto KUnitConversion = KBinary<Type::UnitConversion>();
constexpr auto KEqual = KBinary<Type::Equal>();
constexpr auto KNotEqual = KBinary<Type::NotEqual>();
constexpr auto KSuperior = KBinary<Type::Superior>();
constexpr auto KInferior = KBinary<Type::Inferior>();
constexpr auto KSuperiorEqual = KBinary<Type::SuperiorEqual>();
constexpr auto KInferiorEqual = KBinary<Type::InferiorEqual>();

constexpr auto KDiff = KFixedArity<3, Type::Diff>();
constexpr auto KListSlice = KFixedArity<3, Type::ListSlice>();
constexpr auto KListSequence = KFixedArity<3, Type::ListSequence>();

constexpr auto KSum = KFixedArity<4, Type::Sum>();
constexpr auto KProduct = KFixedArity<4, Type::Product>();
constexpr auto KIntegral = KFixedArity<4, Type::Integral>();
constexpr auto KNthDiff = KFixedArity<4, Type::NthDiff>();

constexpr auto KAdd = KNAry<Type::Add>();
constexpr auto KMult = KNAry<Type::Mult>();
constexpr auto KList = KNAry<Type::List>();
constexpr auto KSet = KNAry<Type::Set>();
constexpr auto KPiecewise = KNAry<Type::Piecewise>();

template <uint8_t Id, uint8_t sign>
constexpr auto KVar = KTree<Type::Var, Id, sign>();

// Discrete local variable
constexpr auto KVarK = KVar<Parametric::k_localVariableId,
                            Parametric::k_discreteVariableSign.getValue()>;

// Continuous local variable
constexpr auto KVarX = KVar<Parametric::k_localVariableId,
                            Parametric::k_continuousVariableSign.getValue()>;

// Default UserSymbol in functions
// TODO: Try to limit the usage of UCodePointUnknown
constexpr auto KUnknownSymbol =
    KTree<Type::UserSymbol, 2, static_cast<uint8_t>(UCodePointUnknown), 0>();

// Booleans
constexpr auto KFalse = KTree<Type::False>();
constexpr auto KTrue = KTree<Type::True>();

constexpr auto KLogicalNot = KUnary<Type::LogicalNot>();

constexpr auto KLogicalAnd = KBinary<Type::LogicalAnd>();
constexpr auto KLogicalOr = KBinary<Type::LogicalOr>();
constexpr auto KLogicalXor = KBinary<Type::LogicalXor>();
constexpr auto KLogicalNor = KBinary<Type::LogicalNor>();
constexpr auto KLogicalNand = KBinary<Type::LogicalNand>();

template <uint8_t Rows, uint8_t Cols>
struct KMatrix {
  template <KTreeConcept... CTS>
    requires(sizeof...(CTS) == Rows * Cols)
  consteval auto operator()(CTS...) const {
    return Concat<decltype(node), CTS...>();
  }

  static constexpr KTree<Type::Matrix, Rows, Cols> node{};

  template <class... Args>
    requires HasATreeConcept<Args...>
  consteval const Tree* operator()(Args... args) const {
    return KTree<>();
  }
};

/* if you want to add operator+ and so on, you can revert them from the commit
 * [poincare_junior] Split tree_constructor.h */

// Alias only for readability
template <uint8_t... Values>
using Exponents = KTree<Values...>;

template <KTreeConcept Exp, KTreeConcept... CTS>
static consteval auto KPol(Exp exponents, CTS...) {
  constexpr uint8_t Size = sizeof...(CTS);
  static_assert(
      Exp::k_size == Size - 1,
      "Number of children and exponents do not match in constant polynomial");
  return Concat<KTree<Type::Polynomial, Size>, Exp, CTS...>();
}

/* Integer literals are used to represent numerical constants of the code (like
 * 2_e) temporarily before they are cast to Trees, to allow writing -2_e. */

/* Each literal is defined with a Representation empty struct that is in charge
 * of inheriting the correct KTree according to the template value and a
 * Literal struct that adds features on top of Representation. */

template <int64_t V>
struct IntegerRepresentation;

template <int64_t V>
struct IntegerLiteral : IntegerRepresentation<V> {
  // IntegerLiteral inherits KTree via IntegerRepresentation
  consteval IntegerLiteral<-V> operator-() { return IntegerLiteral<-V>(); }
  // Note : we could decide to implement constant propagation operators here
};

/* Specializations of IntegerRepresentation to create the smallest Tree that can
 * represent the Literal */

template <>
struct IntegerRepresentation<0> : KTree<Type::Zero> {};
template <>
struct IntegerRepresentation<1> : KTree<Type::One> {};
template <>
struct IntegerRepresentation<-1> : KTree<Type::MinusOne> {};
template <>
struct IntegerRepresentation<2> : KTree<Type::Two> {};

template <int64_t V>
  requires(V >= INT8_MIN && V <= INT8_MAX)
struct IntegerRepresentation<V> : KTree<Type::IntegerShort, V> {};

/* This macro generated code adds deduction guides to construct an IntegerBig
 * with N blocks when V needs N bytes to be represented, for N from 1 to 8 and
 * for negative and positive integers.
 *
 * A single guide looks like this:
 *
 *  template <int64_t V>
 *  requires(V > INT8_MAX && Integer::NumberOfDigits(V) == N)
 *  struct IntegerRepresentation<V>
 *      : KTree<Type::IntegerPosBig, N, OMG::BitHelper::getByteAtIndex(V, 0),
 *                                           ...
 *                                           OMG::BitHelper::getByteAtIndex(V,
 * N-1)>
 * {};
 */

#define SPECIALIZATIONS                               \
  GUIDE(1, B(0));                                     \
  GUIDE(2, B(0), B(1));                               \
  GUIDE(3, B(0), B(1), B(2));                         \
  GUIDE(4, B(0), B(1), B(2), B(3));                   \
  GUIDE(5, B(0), B(1), B(2), B(3), B(4));             \
  GUIDE(6, B(0), B(1), B(2), B(3), B(4), B(5));       \
  GUIDE(7, B(0), B(1), B(2), B(3), B(4), B(5), B(6)); \
  GUIDE(8, B(0), B(1), B(2), B(3), B(4), B(5), B(6), B(7));

// IntegerPosBig
#define GUIDE(N, ...)                                                 \
  template <int64_t V>                                                \
    requires(V > INT8_MAX && OMG::Arithmetic::NumberOfDigits(V) == N) \
  struct IntegerRepresentation<V>                                     \
      : KTree<Type::IntegerPosBig, N, __VA_ARGS__> {};

#define B(I) OMG::BitHelper::getByteAtIndex(V, I)

SPECIALIZATIONS;

#undef B
#undef GUIDE

// IntegerNegBig
#define GUIDE(N, ...)                                                  \
  template <int64_t V>                                                 \
    requires(V < INT8_MIN && OMG::Arithmetic::NumberOfDigits(-V) == N) \
  struct IntegerRepresentation<V>                                      \
      : KTree<Type::IntegerNegBig, N, __VA_ARGS__> {};

#define B(I) OMG::BitHelper::getByteAtIndex(-V, I)

SPECIALIZATIONS;

#undef B
#undef GUIDE
#undef SPECIALIZATIONS

/* Rationals literals are written 2_e / 3_e */

template <int64_t N, int64_t D>
struct RationalRepresentation;

template <int64_t N, int64_t D>
struct RationalLiteral : RationalRepresentation<N, D> {
  consteval auto operator-() { return RationalLiteral<-N, D>(); }
};

template <int64_t N, int64_t D>
  requires(D > 0 && OMG::Arithmetic::GcdI64(N, D) == 1)
consteval auto operator/(IntegerLiteral<N> a, IntegerLiteral<D> b) {
  return RationalLiteral<N, D>();
}

template <>
struct RationalRepresentation<1, 2> : KTree<Type::Half> {};

template <int64_t N, int64_t D>
  requires(N >= INT8_MIN && N <= INT8_MAX && D > 0 && D <= UINT8_MAX)
struct RationalRepresentation<N, D> : KTree<Type::RationalShort, N, D> {};

// TODO specializations for RationalNegBig and RationalPosBig

// TODO: move in OMG?
/* Read decimal number in str as an int, ignoring decimal point "1.2" => 12 */
constexpr static uint64_t IntegerValue(const char* str, size_t size) {
  uint64_t value = 0;
  for (size_t i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      continue;
    }
    uint8_t digit = OMG::Print::DigitForCharacter(str[i]);
    // No overflow
    assert(value <= (UINT64_MAX - digit) / 10);
    value = 10 * value + digit;
  }
  return value;
}

/* Find decimal point in str or return size */
constexpr static size_t DecimalPointIndex(const char* str, size_t size) {
  for (size_t i = 0; i < size - 1; i++) {
    if (str[i] == '.') {
      return i;
    }
  }
  return size;
}

template <class Float>
constexpr static Float FloatValue(const char* str, size_t size) {
  Float value = 0;
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
      base *= 10;
      value += digit / base;
    }
  }
  return value;
}

/* A template <float V> would be cool but support for this is poor yet so we
 * have to store the bit representation of the float as an Int. */

template <class Int, Int V>
struct FloatRepresentation;

template <class Int, Int V>
struct FloatLiteral : FloatRepresentation<Int, V> {
  // Since we are using the representation we have to manually flip the sign bit
  static constexpr Int SignBitMask = Int(1) << (sizeof(Int) * 8 - 1);
  consteval auto operator-() { return FloatLiteral<Int, V ^ SignBitMask>(); }
};

template <uint32_t V>
struct FloatRepresentation<uint32_t, V>
    : KTree<Type::SingleFloat, OMG::BitHelper::getByteAtIndex(V, 0),
            OMG::BitHelper::getByteAtIndex(V, 1),
            OMG::BitHelper::getByteAtIndex(V, 2),
            OMG::BitHelper::getByteAtIndex(V, 3)> {};

template <uint64_t V>
struct FloatRepresentation<uint64_t, V>
    : KTree<Type::DoubleFloat, OMG::BitHelper::getByteAtIndex(V, 0),
            OMG::BitHelper::getByteAtIndex(V, 1),
            OMG::BitHelper::getByteAtIndex(V, 2),
            OMG::BitHelper::getByteAtIndex(V, 3),
            OMG::BitHelper::getByteAtIndex(V, 4),
            OMG::BitHelper::getByteAtIndex(V, 5),
            OMG::BitHelper::getByteAtIndex(V, 6),
            OMG::BitHelper::getByteAtIndex(V, 7)> {};

template <class Float, class Int, char... C>
consteval auto FloatLiteralOperator() {
  constexpr const char value[] = {C..., '\0'};
  return FloatLiteral<Int, std::bit_cast<Int>(
                               FloatValue<Float>(value, sizeof...(C) + 1))>();
}

template <char... C>
consteval auto operator"" _fe() {
  return FloatLiteralOperator<float, uint32_t, C...>();
}

template <char... C>
consteval auto operator"" _de() {
  return FloatLiteralOperator<double, uint64_t, C...>();
}

template <char... C>
consteval auto operator"" _e() {
  constexpr const char value[] = {C..., '\0'};
  constexpr size_t size = sizeof...(C) + 1;
  constexpr size_t decimalPointIndex = DecimalPointIndex(value, size);
  constexpr auto digits = IntegerLiteral<IntegerValue(value, size)>();
  if constexpr (decimalPointIndex < size) {
    return Concat<KTree<Type::Decimal,
                        /* -1 for the . and -1 for the \0 */
                        size - 2 - decimalPointIndex>,
                  decltype(KTree(digits))>();
  } else {
    return digits;
  }
}

// specialized from
// https://stackoverflow.com/questions/60434033/how-do-i-expand-a-compile-time-stdarray-into-a-parameter-pack/60440611#60440611

template <String S,
          typename IS = decltype(std::make_index_sequence<S.size()>())>
struct Variable;

template <String S, std::size_t... I>
struct Variable<S, std::index_sequence<I...>> {
  static_assert(!OMG::Print::IsDigit(S[0]),
                "Integer literals should be written without quotes");
  using tree = KTree<Type::UserSymbol, sizeof...(I), S[I]...>;
};

template <String S>
consteval auto operator"" _e() {
  return typename Variable<S>::tree();
}

}  // namespace KTrees
}  // namespace Poincare::Internal

#endif

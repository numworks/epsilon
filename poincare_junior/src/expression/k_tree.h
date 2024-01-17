#ifndef POINCARE_EXPRESSION_K_TREE_H
#define POINCARE_EXPRESSION_K_TREE_H

#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/parametric.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/memory/k_tree.h>

#include <bit>

namespace PoincareJ {

// Constructors

constexpr auto KUndef = KTree<BlockType::Undefined>();
/* TODO: Remove KHalf and use 1_e/2_e. Also ensure unreduced KTree rational are
 * either forbidden or properly handled. */
constexpr auto KHalf = KTree<BlockType::Half>();
constexpr auto KNonreal = KTree<BlockType::Nonreal>();
constexpr auto KInf = KTree<BlockType::Infinity>();

constexpr auto KAbs = KUnary<BlockType::Abs>();
constexpr auto KCos = KUnary<BlockType::Cosine>();
constexpr auto KSin = KUnary<BlockType::Sine>();
constexpr auto KTan = KUnary<BlockType::Tangent>();
constexpr auto KTanRad = KUnary<BlockType::TangentRad>();
constexpr auto KACos = KUnary<BlockType::ArcCosine>();
constexpr auto KASin = KUnary<BlockType::ArcSine>();
constexpr auto KATan = KUnary<BlockType::ArcTangent>();
constexpr auto KATanRad = KUnary<BlockType::ArcTangentRad>();
constexpr auto KLog = KUnary<BlockType::Log>();
constexpr auto KLn = KUnary<BlockType::Ln>();
constexpr auto KExp = KUnary<BlockType::Exponential>();
constexpr auto KFact = KUnary<BlockType::Factorial>();
constexpr auto KSqrt = KUnary<BlockType::SquareRoot>();
constexpr auto KRe = KUnary<BlockType::RealPart>();
constexpr auto KIm = KUnary<BlockType::ImaginaryPart>();
constexpr auto KArg = KUnary<BlockType::ComplexArgument>();
constexpr auto KConj = KUnary<BlockType::Conjugate>();
constexpr auto KOpposite = KUnary<BlockType::Opposite>();
constexpr auto KFloor = KUnary<BlockType::Floor>();
constexpr auto KCeil = KUnary<BlockType::Ceiling>();
constexpr auto KFrac = KUnary<BlockType::FracPart>();
constexpr auto KListSum = KUnary<BlockType::ListSum>();
constexpr auto KMin = KUnary<BlockType::Minimum>();
constexpr auto KMax = KUnary<BlockType::Maximum>();
constexpr auto KSec = KUnary<BlockType::Secant>();
constexpr auto KCsc = KUnary<BlockType::Cosecant>();
constexpr auto KCot = KUnary<BlockType::Cotangent>();
constexpr auto KArcSec = KUnary<BlockType::ArcSecant>();
constexpr auto KArcCsc = KUnary<BlockType::ArcCosecant>();
constexpr auto KArcCot = KUnary<BlockType::ArcCotangent>();
constexpr auto KCosh = KUnary<BlockType::HyperbolicCosine>();
constexpr auto KSinh = KUnary<BlockType::HyperbolicSine>();
constexpr auto KTanh = KUnary<BlockType::HyperbolicTangent>();
constexpr auto KArCosh = KUnary<BlockType::HyperbolicArcCosine>();
constexpr auto KArSinh = KUnary<BlockType::HyperbolicArcSine>();
constexpr auto KArTanh = KUnary<BlockType::HyperbolicArcTangent>();

constexpr auto KATrig = KBinary<BlockType::ATrig>();
constexpr auto KComplex = KBinary<BlockType::Complex>();
constexpr auto KLogarithm = KBinary<BlockType::Logarithm>();
constexpr auto KTrig = KBinary<BlockType::Trig>();
constexpr auto KTrigDiff = KBinary<BlockType::TrigDiff>();
constexpr auto KDiv = KBinary<BlockType::Division>();
constexpr auto KSub = KBinary<BlockType::Subtraction>();
constexpr auto KPow = KBinary<BlockType::Power>();
constexpr auto KPowReal = KBinary<BlockType::PowerReal>();
constexpr auto KPowMatrix = KBinary<BlockType::PowerMatrix>();
constexpr auto KDep = KBinary<BlockType::Dependency>();
constexpr auto KRound = KBinary<BlockType::Round>();
constexpr auto KMean = KBinary<BlockType::Mean>();
constexpr auto KBinomial = KBinary<BlockType::Binomial>();
constexpr auto KPermute = KBinary<BlockType::Permute>();

constexpr auto KDiff = KFixedArity<3, BlockType::Derivative>();
constexpr auto KSum = KFixedArity<4, BlockType::Sum>();
constexpr auto KProduct = KFixedArity<4, BlockType::Product>();
constexpr auto KIntegral = KFixedArity<4, BlockType::Integral>();

constexpr auto KAdd = KNAry<BlockType::Addition>();
constexpr auto KMult = KNAry<BlockType::Multiplication>();
constexpr auto KList = KNAry<BlockType::List>();
constexpr auto KSet = KNAry<BlockType::Set>();

template <uint8_t Id, uint8_t sign>
constexpr auto KVar = KTree<BlockType::Variable, Id, sign>();

// Discrete local variable
constexpr auto KVarK = KVar<Parametric::k_localVariableId,
                            Parametric::k_discreteVariableSign.getValue()>;

// Continuous local variable
constexpr auto KVarX = KVar<Parametric::k_localVariableId,
                            Parametric::k_continuousVariableSign.getValue()>;
;

template <uint8_t Rows, uint8_t Cols>
class KMatrix {
 public:
  template <TreeCompatibleConcept... CTS>
  consteval auto operator()(CTS... args) const {
    return concat(KTree(args)...);
  }

  static constexpr KTree<BlockType::Matrix, Rows, Cols> node{};

  template <class... Args>
    requires HasATreeConcept<Args...>
  consteval const Tree* operator()(Args... args) const {
    return KTree<>();
  }

 private:
  template <TreeConcept... CTS>
    requires(sizeof...(CTS) == Rows * Cols)
  consteval auto concat(CTS...) const {
    return Concat<decltype(node), CTS...>();
  }
};

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

/* Integer litterals are used to represent numerical constants of the code (like
 * 2_e) temporarily before they are cast to Trees, to allow writing -2_e. */

template <int64_t V>
class IntegerLitteral : public TreeCompatible<IntegerLitteral<V>> {
 public:
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

template <int64_t V>
  requires(V >= INT8_MIN && V <= INT8_MAX)
KTree(IntegerLitteral<V>) -> KTree<BlockType::IntegerShort, V>;

/* This macro generated code adds deduction guides to construct an IntegerBig
 * with N blocks when V needs N bytes to be represented, for N from 1 to 8 and
 * for negative and positive integers.
 *
 * A single guide looks like this:
 *
 *  template <int64_t V>
 *  requires(V > INT8_MAX && Integer::NumberOfDigits(V) == N)
 *      KTree(IntegerLitteral<V>)
 *  ->KTree<BlockType::IntegerPosBig, N, Bit::getByteAtIndex(V, 0),
 *                                       ...
 *                                       Bit::getByteAtIndex(V, N-1)>;
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
#define GUIDE(N, ...)                                         \
  template <int64_t V>                                        \
    requires(V > INT8_MAX && Integer::NumberOfDigits(V) == N) \
  KTree(IntegerLitteral<V>)                                   \
      -> KTree<BlockType::IntegerPosBig, N, __VA_ARGS__>;

#define B(I) Bit::getByteAtIndex(V, I)

SPECIALIZATIONS;

#undef B
#undef GUIDE

// IntegerNegBig
#define GUIDE(N, ...)                                          \
  template <int64_t V>                                         \
    requires(V < INT8_MIN && Integer::NumberOfDigits(-V) == N) \
  KTree(IntegerLitteral<V>)                                    \
      -> KTree<BlockType::IntegerNegBig, N, __VA_ARGS__>;

#define B(I) Bit::getByteAtIndex(-V, I)

SPECIALIZATIONS;

#undef B
#undef GUIDE
#undef SPECIALIZATIONS

/* Rationals litterals are written 2_e / 3_e */

template <int64_t N, int64_t D>
class RationalLitteral : public TreeCompatible<RationalLitteral<N, D>> {
 public:
  consteval auto operator-() { return RationalLitteral<-N, D>(); }
};

template <int64_t N, int64_t D>
  requires(D > 0)
consteval auto operator/(IntegerLitteral<N> a, IntegerLitteral<D> b) {
  return RationalLitteral<N, D>();
}

KTree(RationalLitteral<1, 2>)->KTree<BlockType::Half>;

template <int64_t N, int64_t D>
  requires(N >= INT8_MIN && N <= INT8_MAX && D > 0 && D <= UINT8_MAX)
KTree(RationalLitteral<N, D>) -> KTree<BlockType::RationalShort, N, D>;

// TODO guides for RationalNegBig and RationalPosBig

/* Named constants */

constexpr KTree π_e =
    KTree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::Pi)>();

constexpr KTree e_e =
    KTree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::E)>();

constexpr KTree i_e =
    KTree<BlockType::Constant, static_cast<uint8_t>(Constant::Type::I)>();

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
class FloatLitteral : public AbstractTreeCompatible {
 public:
  template <Block... B>
  consteval operator KTree<B...>() {
    return KTree<B...>();
  }

  constexpr operator const Tree*() const {
    return KTree(FloatLitteral<Int, V>());
  }
  const Tree* operator->() const { return KTree(FloatLitteral<Int, V>()); }

  // Since we are using the representation we have to manually flip the sign bit
  static constexpr Int SignBitMask = Int(1) << (sizeof(Int) * 8 - 1);
  consteval auto operator-() { return FloatLitteral<Int, V ^ SignBitMask>(); }
};

template <uint32_t V>
KTree(FloatLitteral<uint32_t, V>)
    -> KTree<BlockType::SingleFloat, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
             Bit::getByteAtIndex(V, 3)>;

template <uint64_t V>
KTree(FloatLitteral<uint64_t, V>)
    -> KTree<BlockType::DoubleFloat, Bit::getByteAtIndex(V, 0),
             Bit::getByteAtIndex(V, 1), Bit::getByteAtIndex(V, 2),
             Bit::getByteAtIndex(V, 3), Bit::getByteAtIndex(V, 4),
             Bit::getByteAtIndex(V, 5), Bit::getByteAtIndex(V, 6),
             Bit::getByteAtIndex(V, 7)>;

template <class Float, class Int, char... C>
consteval auto FloatLitteralOperator() {
  constexpr const char value[] = {C..., '\0'};
  return FloatLitteral<Int, std::bit_cast<Int>(
                                FloatValue<Float>(value, sizeof...(C) + 1))>();
}

template <char... C>
consteval auto operator"" _fe() {
  return FloatLitteralOperator<float, uint32_t, C...>();
}

template <char... C>
consteval auto operator"" _de() {
  return FloatLitteralOperator<double, uint64_t, C...>();
}

template <char... C>
consteval auto operator"" _e() {
  constexpr const char value[] = {C..., '\0'};
  constexpr size_t size = sizeof...(C) + 1;
  constexpr size_t decimalPointIndex = DecimalPointIndex(value, size);
  constexpr auto digits = IntegerLitteral<IntegerValue(value, size)>();
  if constexpr (decimalPointIndex < size) {
    return Concat<KTree<BlockType::Decimal,
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

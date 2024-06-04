#include <poincare/old/based_integer.h>
#include <poincare/old/decimal.h>
#include <poincare/old/derivative.h>
#include <poincare/old/float.h>
#include <poincare/old/infinity.h>
#include <poincare/old/integer.h>
#include <poincare/old/number.h>
#include <poincare/old/rational.h>
#include <poincare/old/undefined.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <utility>

namespace Poincare {

bool NumberNode::derivate(const ReductionContext &reductionContext,
                          Symbol symbol, OExpression symbolValue) {
  return Number(this).derivate(reductionContext, symbol, symbolValue);
}

Number Number::ParseNumber(const char *integralPart, size_t integralLength,
                           const char *decimalPart, size_t decimalLength,
                           bool exponentIsNegative, const char *exponentPart,
                           size_t exponentLength) {
  // Remove useless zeros
  while (integralLength > 1 && integralPart[0] == '0') {
    integralPart++;
    integralLength--;
  }
  while (decimalLength > 1 && decimalPart[decimalLength - 1] == '0') {
    decimalLength--;
  }
  // Integer
  if (exponentLength == 0 && decimalLength == 0) {
    Integer i(integralPart, integralLength, false);
    if (i.isNotParsable(integralLength)) {
      return Number::FloatNumber(i.approximate<double>());
    } else {
      return BasedInteger::Builder(i, OMG::Base::Decimal);
    }
  }
  int exp;
  // Avoid overflowing int
  if (exponentLength < Decimal::k_maxExponentLength) {
    exp = Decimal::Exponent(integralPart, integralLength, decimalPart,
                            decimalLength, exponentPart, exponentLength,
                            exponentIsNegative);
  } else {
    exp = exponentIsNegative ? -1 : 1;
  }
  // Avoid Decimal with exponent > k_maxExponentLength
  if (exponentLength >= Decimal::k_maxExponentLength ||
      exp > Decimal::k_maxExponent || exp < -Decimal::k_maxExponent) {
    if (exp < 0) {
      return Decimal::Builder(0.0);
    } else {
      return Infinity::Builder(false);
    }
  }
  return Decimal::Builder(integralPart, integralLength, decimalPart,
                          decimalLength, exp);
}

template <typename T>
Number Number::DecimalNumber(T f) {
  if (std::isnan(f)) {
    return OUndefined::Builder();
  }
  if (std::isinf(f)) {
    return Infinity::Builder(f < static_cast<T>(0.0));
  }
  return Decimal::Builder(f);
}

template <typename T>
Number Number::FloatNumber(T f) {
  if (std::isnan(f)) {
    return OUndefined::Builder();
  } else if (std::isinf(f)) {
    return Infinity::Builder(f < static_cast<T>(0.0));
  } else {
    return Float<T>::Builder(f);
  }
}

Number Number::BinaryOperation(const Number &i, const Number &j,
                               RationalBinaryOperation rationalOp,
                               DoubleBinaryOperation doubleOp) {
  if (i.otype() == ExpressionNode::Type::Rational &&
      j.otype() == ExpressionNode::Type::Rational) {
    // Rational + Rational
    Rational a = rationalOp(static_cast<const Rational &>(i),
                            static_cast<const Rational &>(j));
    if (!a.numeratorOrDenominatorIsInfinity()) {
      return std::move(a);
    }
  }
#if 0  // TODO_PCJ: doubleApproximation has been removed
  /* At least one of the operands is Undefined/Infinity/Float, or the Rational
   * addition overflowed */
  double a = doubleOp(i.node()->doubleApproximation(),
                      j.node()->doubleApproximation());
  return FloatNumber(a);
#else
  assert(false);
  return FloatNumber(0.0);
#endif
}

Number Number::Addition(const Number &i, const Number &j) {
  return BinaryOperation(i, j, Rational::Addition,
                         [](double a, double b) { return a + b; });
}

Number Number::Multiplication(const Number &i, const Number &j) {
  return BinaryOperation(i, j, Rational::Multiplication,
                         [](double a, double b) { return a * b; });
}

Number Number::Power(const Number &i, const Number &j) {
  return BinaryOperation(
      i, j,
      /* Rational ^ Rational should not be handled here, but in
       * Power::shallowReduce */
      [](const Rational &i, const Rational &j) {
        assert(false);
        return Rational::Builder(Integer::Overflow(false));
      },
      [](double a, double b) { return std::pow(a, b); });
}

int Number::NaturalOrder(const Number &i, const Number &j) {
  if (i.node()->otype() == ExpressionNode::Type::Rational &&
      j.node()->otype() == ExpressionNode::Type::Rational) {
    // Rational + Rational
    return Rational::NaturalOrder(static_cast<const Rational &>(i),
                                  static_cast<const Rational &>(j));
  }
  /* One of the operand is Undefined/Infinity/Float or the Rational addition
   * overflowed. */
#if 0  // TODO_PCJ: doubleApproximation has been removed
  if (i.node()->doubleApproximation() < j.node()->doubleApproximation()) {
    return -1;
  } else if (i.node()->doubleApproximation() ==
             j.node()->doubleApproximation()) {
    return 0;
  } else {
    assert(i.node()->doubleApproximation() > j.node()->doubleApproximation());
    return 1;
  }
#else
  assert(false);
  return 0;
#endif
}

bool Number::derivate(const ReductionContext &reductionContext, Symbol symbol,
                      OExpression symbolValue) {
  replaceWithInPlace(Rational::Builder(0));
  return true;
}

template Number Number::DecimalNumber<float>(float);
template Number Number::DecimalNumber<double>(double);
template Number Number::FloatNumber<float>(float);
template Number Number::FloatNumber<double>(double);
}  // namespace Poincare

#include <poincare/number.h>
#include <poincare/based_integer.h>
#include <poincare/decimal.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/derivative.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>
#include <utility>

namespace Poincare {

double NumberNode::doubleApproximation() const {
  switch (type()) {
    case Type::Undefined:
    case Type::Nonreal:
      return NAN;
    case Type::Infinity:
      assert(Number(this).isPositive() == TrinaryBoolean::False || Number(this).isPositive() == TrinaryBoolean::True);
      return Number(this).isPositive() == TrinaryBoolean::False ? -INFINITY : INFINITY;
    case Type::Float:
      return static_cast<const FloatNode<float> *>(this)->value();
    case Type::Double:
      return static_cast<const FloatNode<double> *>(this)->value();
    case Type::Rational:
      return static_cast<const RationalNode *>(this)->templatedApproximate<double>();
    case Type::BasedInteger:
      return static_cast<const BasedIntegerNode *>(this)->templatedApproximate<double>();
    case Type::Decimal:
      return static_cast<const DecimalNode *>(this)->templatedApproximate<double>();
    default:
      assert(false);
      return 0.0;
  }
}

bool NumberNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Number(this).derivate(reductionContext, symbol, symbolValue);
}

Number Number::ParseNumber(const char * integralPart, size_t integralLength, const char * decimalPart, size_t decimalLength, bool exponentIsNegative, const char * exponentPart, size_t exponentLength) {
  // Integer
  if (exponentLength == 0 && decimalLength == 0) {
    Integer i(integralPart, integralLength, false);
    if (i.isNotParsable(integralLength)) {
      return Number::FloatNumber(i.approximate<double>());
    } else {
      return BasedInteger::Builder(i, Integer::Base::Decimal);
    }
  }
  int exp;
  // Avoid overflowing int
  if (exponentLength < Decimal::k_maxExponentLength) {
    exp = Decimal::Exponent(integralPart, integralLength, decimalPart, decimalLength, exponentPart, exponentLength, exponentIsNegative);
  } else {
    exp = exponentIsNegative ? -1 : 1;
  }
  // Avoid Decimal with exponent > k_maxExponentLength
  if (exponentLength >= Decimal::k_maxExponentLength || exp > Decimal::k_maxExponent || exp < -Decimal::k_maxExponent) {
    if (exp < 0) {
      return Decimal::Builder(0.0);
    } else {
      return Infinity::Builder(false);
    }
  }
  return Decimal::Builder(integralPart, integralLength, decimalPart, decimalLength, exp);
}

template <typename T>
Number Number::DecimalNumber(T f) {
  if (std::isnan(f)) {
    return Undefined::Builder();
  }
  if (std::isinf(f)) {
    return Infinity::Builder(f < static_cast<T>(0.0));
  }
  return Decimal::Builder(f);
}

template <typename T>
Number Number::FloatNumber(T f) {
  if (std::isnan(f)) {
    return Undefined::Builder();
  } else if (std::isinf(f)) {
    return Infinity::Builder(f < static_cast<T>(0.0));
  } else {
    return Float<T>::Builder(f);
  }
}

Number Number::BinaryOperation(const Number & i, const Number & j, RationalBinaryOperation rationalOp, DoubleBinaryOperation doubleOp) {
  if (i.type() == ExpressionNode::Type::Rational && j.type() == ExpressionNode::Type::Rational) {
    // Rational + Rational
    Rational a = rationalOp(static_cast<const Rational&>(i), static_cast<const Rational&>(j));
    if (!a.numeratorOrDenominatorIsInfinity()) {
      return std::move(a);
    }
  }
  /* At least one of the operands is Undefined/Infinity/Float, or the Rational
   * addition overflowed */
  double a = doubleOp(i.node()->doubleApproximation(), j.node()->doubleApproximation());
  return FloatNumber(a);
}

Number Number::Addition(const Number & i, const Number & j) {
  return BinaryOperation(i, j, Rational::Addition, [](double a, double b) { return a+b; });
}

Number Number::Multiplication(const Number & i, const Number & j) {
  return BinaryOperation(i, j, Rational::Multiplication, [](double a, double b) { return a*b; });
}

Number Number::Power(const Number & i, const Number & j) {
  return BinaryOperation(i, j,
      // Rational ^ Rational should not be handled here, but in Power::shallowReduce
      [](const Rational & i, const Rational & j) {
        assert(false);
        return Rational::Builder(Integer::Overflow(false));
      },
      [](double a, double b) {
        return std::pow(a, b);
      }
    );
}

int Number::NaturalOrder(const Number & i, const Number & j) {
  if (i.node()->type() == ExpressionNode::Type::Rational && j.node()->type() == ExpressionNode::Type::Rational) {
  // Rational + Rational
    return Rational::NaturalOrder(static_cast<const Rational&>(i), static_cast<const Rational&>(j));
  }
  // one of the operand is Undefined/Infinity/Float or the Rational addition overflowed
  if (i.node()->doubleApproximation() < j.node()->doubleApproximation()) {
    return -1;
  } else if (i.node()->doubleApproximation() == j.node()->doubleApproximation()) {
    return 0;
  } else {
    assert(i.node()->doubleApproximation() > j.node()->doubleApproximation());
    return 1;
  }
}

bool Number::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  replaceWithInPlace(Rational::Builder(0));
  return true;
}

template Number Number::DecimalNumber<float>(float);
template Number Number::DecimalNumber<double>(double);
template Number Number::FloatNumber<float>(float);
template Number Number::FloatNumber<double>(double);
}

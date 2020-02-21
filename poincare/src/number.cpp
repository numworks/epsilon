#include <poincare/number.h>
#include <poincare/based_integer.h>
#include <poincare/decimal.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
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
    case Type::Unreal:
      return NAN;
    case Type::Infinity:
      assert(Number(this).sign() == Sign::Negative || Number(this).sign() == Sign::Positive);
      return Number(this).sign() == Sign::Negative ? -INFINITY : INFINITY;
    case Type::Float:
      if (size() == sizeof(FloatNode<float>)) {
        return static_cast<const FloatNode<float> *>(this)->value();
      } else {
        assert(size() == sizeof(FloatNode<double>));
        return static_cast<const FloatNode<double> *>(this)->value();
      }
    case Type::Rational:
      return static_cast<const RationalNode *>(this)->templatedApproximate<double>();
    case Type::BasedInteger:
      return static_cast<const BasedIntegerNode *>(this)->templatedApproximate<double>();
    default:
      assert(false);
      return 0.0;
  }
}

Number Number::ParseNumber(const char * integralPart, size_t integralLength, const char * decimalPart, size_t decimalLenght, bool exponentIsNegative, const char * exponentPart, size_t exponentLength) {
  // Integer
  if (exponentLength == 0 && decimalLenght == 0) {
    Integer i(integralPart, integralLength, false);
    if (!i.isOverflow()) {
      return BasedInteger::Builder(i, Integer::Base::Decimal);
    }
  }
  int exp;
  // Avoid overflowing int
  if (exponentLength < Decimal::k_maxExponentLength) {
    exp = Decimal::Exponent(integralPart, integralLength, decimalPart, decimalLenght, exponentPart, exponentLength, exponentIsNegative);
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
  return Decimal::Builder(integralPart, integralLength, decimalPart, decimalLenght, exp);
}

template <typename T>
Number Number::DecimalNumber(T f) {
  if (std::isnan(f)) {
    return Undefined::Builder();
  }
  if (std::isinf(f)) {
    return Infinity::Builder(f < (T)0.0);
  }
  return Decimal::Builder(f);
}

Number Number::FloatNumber(double d) {
  if (std::isnan(d)) {
    return Undefined::Builder();
  } else if (std::isinf(d)) {
    return Infinity::Builder(d < 0.0);
  } else {
    return Float<double>::Builder(d);
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
      // Special case for Rational^Rational: we escape to Float if the index is not an Integer
      [](const Rational & i, const Rational & j) {
        if (!j.isInteger()) {
          // We return an overflown result to reach the escape case Float+Float
          return Rational::Builder(Integer::Overflow(false));
        }
        return Rational::IntegerPower(i, j.signedIntegerNumerator());
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

template Number Number::DecimalNumber<float>(float);
template Number Number::DecimalNumber<double>(double);
}

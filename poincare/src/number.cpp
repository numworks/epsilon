#include <poincare/number.h>
#include <poincare/decimal.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/float.h>
#include <poincare/undefined.h>
#include <poincare/infinity.h>

extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

double NumberNode::doubleApproximation() const {
  switch (type()) {
    case Type::Undefined:
      return NAN;
    case Type::Infinity:
      return sign() == Sign::Negative ? -INFINITY : INFINITY;
    case Type::Float:
      if (size() == sizeof(FloatNode<float>)) {
        return static_cast<const FloatNode<float> *>(this)->value();
      } else {
        assert(size() == sizeof(FloatNode<double>));
        return static_cast<const FloatNode<double> *>(this)->value();
      }
    case Type::Rational:
      return static_cast<const RationalNode *>(this)->templatedApproximate<double>();
    default:
      assert(false);
      return 0.0;
  }
}

Number Number::ParseDigits(const char * digits, size_t length) {
  assert(digits[0] != '-');
  const char * integral = digits;
  size_t integralLength = length;
  const char * fractional = strchr(digits, '.');
  size_t fractionalLength = 0;
  if (fractional) {
    integralLength = fractional - integral;
    fractional++;
    fractionalLength = length - integralLength - 1;
  }
  const char * exponent = strchr(digits, Ion::Charset::Exponent);
  size_t exponentLength = 0;
  if (exponent) {
    integralLength = fractional ? integralLength : exponent - integral;
    fractionalLength = fractional ? exponent - fractional : 0;
    exponent++;
    exponentLength = length - integralLength - fractionalLength - (fractional ? 2 : 1);
  }
  // Integer
  if (exponentLength == 0 && fractionalLength == 0) {
    Integer i(integral, integralLength, false);
    if (!i.isInfinity()) {
      return Rational(i);
    }
  }
  int exp;
  // Avoid overflowing int
  if (exponentLength < Decimal::k_maxExponentLength) {
    exp = Decimal::Exponent(integral, integralLength, fractional, fractionalLength, exponent, exponentLength);
  } else {
    assert(exponent);
    exp = exponent[0] == '-' ? -1 : 1;
  }
  // Avoid Decimal with exponent > k_maxExponentLength
  if (exponentLength >= Decimal::k_maxExponentLength || exp > Decimal::k_maxExponent || exp < -Decimal::k_maxExponent) {
    if (exp < 0) {
      return Decimal(0.0);
    } else {
      return Infinity(false);
    }
  }
  return Decimal(integral, integralLength, fractional, fractionalLength, exp);
}

template <typename T>
Number Number::DecimalNumber(T f) {
  if (std::isnan(f)) {
    return Undefined();
  }
  if (std::isinf(f)) {
    return Infinity(f < 0.0);
  }
  return Decimal(f);
}

Number Number::FloatNumber(double d) {
  if (std::isnan(d)) {
    return Undefined();
  } else if (std::isinf(d)) {
    return Infinity(d < 0.0);
  } else {
    return Float<double>(d);
  }
}

Number Number::BinaryOperation(const Number & i, const Number & j, RationalBinaryOperation rationalOp, DoubleBinaryOperation doubleOp) {
  if (i.type() == ExpressionNode::Type::Rational && j.type() == ExpressionNode::Type::Rational) {
    // Rational + Rational
    Rational a = rationalOp(static_cast<const Rational&>(i), static_cast<const Rational&>(j));
    if (!a.numeratorOrDenominatorIsInfinity()) {
      return a;
    }
  }
  // one of the operand is Undefined/Infinity/Float or the Rational addition overflowed
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
        if (!j.integerDenominator().isOne()) {
          // We return an overflown result to reach the escape case Float+Float
          return Rational(Integer::Overflow(false));
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

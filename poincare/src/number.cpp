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
      if (sizeof(*this) == sizeof(FloatNode<float>)) {
        return static_cast<const FloatNode<float> *>(this)->value();
      } else {
        assert(sizeof(*this) == sizeof(FloatNode<double>));
        return static_cast<const FloatNode<double> *>(this)->value();
      }
    case Type::Rational:
      return static_cast<const RationalNode *>(this)->templatedApproximate<double>();
    case Type::Integer:
      return static_cast<const IntegerNode *>(this)->templatedApproximate<double>();
    default:
      assert(false);
      return 0.0;
  }
}

NumberReference createReference(double d) {
  if (std::isnan(d)) {
    return UndefinedReference();
  } else if (std::isinf(d)) {
    return InfinityReference(d < 0.0);
  } else {
    return FloatReference<double>(d);
  }
}

NumberReference NumberReference::Integer(const char * digits, size_t length, bool negative) {
  IntegerReference i(digits, length, negative);
  if (!i.isInfinity()) {
    return i;
  }
  if (digits != nullptr && digits[0] == '-') {
    negative = true;
    digits++;
  }
  if (length > Decimal::k_maxExponentLength) {
    return InfinityReference(negative);
  }
  int exponent = Decimal::Exponent(digits, length, nullptr, 0, nullptr, 0, negative);
  return DecimalReference(digits, length, nullptr, 0, negative, exponent);
}

template <typename T> static NumberReference NumberReference::Decimal(T f) {
  if (std::isnan(f)) {
    return UndefinedReference();
  }
  if (std::isinf(f)) {
    return InfiniteReference(f < 0.0);
  }
  return DecimalReference(f);
}

NumberReference NumberReference::BinaryOperation(const NumberReference i, const NumberReference j, IntegerBinaryOperation integerOp, RationalBinaryOperation rationalOp, DoubleBinaryOperation doubleOp) {
  if (i.isAllocationFailure() || j.isAllocationFailure()) {
    return NumberReference(ExpressionNode::FailedAllocationStaticNode());
  }
  if (i.node()->type() == ExpressionNode::Type::Integer && j.node()->type() == ExpressionNode::Type::Integer) {
  // Integer + Integer
    IntegerReference k = integerOp(IntegerReference(i.node()), IntegerReference(j.node()));
    if (!k.isInfinity()) {
      return k;
    }
  } else if (i.node()->type() == ExpressionNode::Type::Integer && j.node()->type() == ExpressionNode::Type::Rational) {
  // Integer + Rational
    RationalReference r = rationalOp(RationalReference(IntegerReference(i.node())), RationalReference(j.node()));
    if (!r.numeratorOrDenominatorIsInfinity()) {
      return r;
    }
  } else if (i.node()->type() == ExpressionNode::Type::Rational && j.node()->type() == ExpressionNode::Type::Integer) {
  // Rational + Integer
    return NumberReference::BinaryOperation(j, i, integerOp, rationalOp, doubleOp);
  } else if (i.node()->type() == ExpressionNode::Type::Rational && j.node()->type() == ExpressionNode::Type::Rational) {
  // Rational + Rational
    RationalReference a = rationalOp(RationalReference(i.node()), RationalReference(j.node()));
    if (!a.numeratorOrDenominatorIsInfinity()) {
      return a;
    }
  }
  // one of the operand is Undefined/Infinity/Float or the Integer/Rational addition overflowed
  double a = doubleOp(i.numberNode()->doubleApproximation(), j.numberNode()->doubleApproximation());
  return createReference(a);
}

NumberReference NumberReference::Addition(const NumberReference i, const NumberReference j) {
  return BinaryOperation(i, j, IntegerReference::Addition, RationalReference::Addition, [](double a, double b) { return a+b; });
}

NumberReference NumberReference::Multiplication(const NumberReference i, const NumberReference j) {
  return BinaryOperation(i, j, IntegerReference::Multiplication, RationalReference::Multiplication, [](double a, double b) { return a*b; });
}

NumberReference NumberReference::Power(const NumberReference i, const NumberReference j) {
  return BinaryOperation(i, j, IntegerReference::Power,
      // Special case for Rational^Rational: we escape to Float if the index is not an Integer
      [](const RationalReference i, const RationalReference j) {
        if (!j.isAllocationFailure() && j.typedNode()->denominator().isOne()) {
          return RationalReference::IntegerPower(i, j);
        } else {
          // We return an overflown result to reach the escape case Float+Float
          return RationalReference(IntegerReference::Overflow());
        }
      },
      [](double a, double b) {
        return std::pow(a, b);
      }
    );
}

}

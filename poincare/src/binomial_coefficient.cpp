#include <poincare/binomial_coefficient.h>
#include <poincare/complex.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include "layout/parenthesis_layout.h"
#include "layout/grid_layout.h"

extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type BinomialCoefficient::type() const {
  return Type::BinomialCoefficient;
}

Expression * BinomialCoefficient::clone() const {
  BinomialCoefficient * b = new BinomialCoefficient(m_operands, true);
  return b;
}

Expression * BinomialCoefficient::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op0 = editableOperand(0);
  Expression * op1 = editableOperand(1);
  if (op0->type() == Type::Matrix || op1->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
  if (op0->type() == Type::Rational) {
    Rational * r0 = static_cast<Rational *>(op0);
    if (!r0->denominator().isOne() || r0->numerator().isNegative()) {
      return replaceWith(new Undefined(), true);
    }
  }
  if (op1->type() == Type::Rational) {
    Rational * r1 = static_cast<Rational *>(op1);
    if (!r1->denominator().isOne() || r1->numerator().isNegative()) {
      return replaceWith(new Undefined(), true);
    }
  }
  if (op0->type() != Type::Rational || op1->type() != Type::Rational) {
    return this;
  }
  Rational * r0 = static_cast<Rational *>(op0);
  Rational * r1 = static_cast<Rational *>(op1);

  Integer n = r0->numerator();
  Integer k = r1->numerator();
  if (n.isLowerThan(k)) {
    return replaceWith(new Undefined(), true);
  }
  Integer result(1);
  Integer kBis = Integer::Subtraction(n, k);
  k = kBis.isLowerThan(k) ? kBis : k;
  // Out of bounds
  if (Integer(k_maxNumberOfSteps).isLowerThan(k)) {
    return replaceWith(new Undefined(), true);
  }
  int clippedK = k.extractedInt(); // Authorized because k < k_maxNumberOfSteps
  for (int i = 0; i < clippedK; i++) {
    Integer factor = Integer::Division(Integer::Subtraction(n, Integer(i)), Integer::Subtraction(k, Integer(i))).quotient;
    result = Integer::Multiplication(result, factor);
  }
  return replaceWith(new Rational(result), true);
}

ExpressionLayout * BinomialCoefficient::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = operand(0)->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = operand(1)->createLayout(floatDisplayMode, complexFormat);
  return new ParenthesisLayout(new GridLayout(childrenLayouts, 2, 1));
}

template<typename T>
Complex<T> * BinomialCoefficient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Complex<T> * nInput = operand(0)->privateEvaluate(T(), context, angleUnit);
  Complex<T> * kInput = operand(1)->privateEvaluate(T(), context, angleUnit);
  T n = nInput->toScalar();
  T k = kInput->toScalar();
  delete nInput;
  delete kInput;
  k = k > (n-k) ? n-k : k;
  if (isnan(n) || isnan(k) || n != std::round(n) || k != std::round(k) || k > n || k < 0 || n < 0 || k > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T result = 1;
  for (int i = 0; i < k; i++) {
    result *= (n-(T)i)/(k-(T)i);
  }
  return new Complex<T>(Complex<T>::Float(std::round(result)));
}

}


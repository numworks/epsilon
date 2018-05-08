#include <poincare/binomial_coefficient.h>
#include <poincare/undefined.h>
#include <poincare/complex.h>
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
#if MATRIX_EXACT_REDUCING
  if (op0->type() == Type::Matrix || op1->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
#endif
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
  /* if n is too big, we do not reduce to avoid too long computation.
   * The binomial coefficient will be evaluate approximatively later */
  if (Integer(k_maxNValue).isLowerThan(n)) {
    return this;
  }
  Rational result(1);
  Integer kBis = Integer::Subtraction(n, k);
  k = kBis.isLowerThan(k) ? kBis : k;
  int clippedK = k.extractedInt(); // Authorized because k < n < k_maxNValue
  for (int i = 0; i < clippedK; i++) {
    Rational factor = Rational(Integer::Subtraction(n, Integer(i)), Integer::Subtraction(k, Integer(i)));
    result = Rational::Multiplication(result, factor);
  }
  return replaceWith(new Rational(result), true);
}

ExpressionLayout * BinomialCoefficient::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = operand(0)->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = operand(1)->createLayout(floatDisplayMode, complexFormat);
  return new ParenthesisLayout(new GridLayout(childrenLayouts, 2, 1));
}

template<typename T>
Expression * BinomialCoefficient::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * nInput = operand(0)->approximate<T>(context, angleUnit);
  Expression * kInput = operand(1)->approximate<T>(context, angleUnit);
  if (nInput->type() != Type::Complex || kInput->type() != Type::Complex) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T n = static_cast<Complex<T> *>(nInput)->toScalar();
  T k = static_cast<Complex<T> *>(kInput)->toScalar();
  delete nInput;
  delete kInput;
  return new Complex<T>(Complex<T>::Float(compute(k, n)));
}


template<typename T>
T BinomialCoefficient::compute(T k, T n) {
  k = k > (n-k) ? n-k : k;
  if (std::isnan(n) || std::isnan(k) || n != std::round(n) || k != std::round(k) || k > n || k < 0 || n < 0) {
    return NAN;
  }
  T result = 1;
  for (int i = 0; i < k; i++) {
    result *= (n-(T)i)/(k-(T)i);
    if (std::isinf(result) || std::isnan(result)) {
      return result;
    }
  }
  return std::round(result);
}

template double BinomialCoefficient::compute(double k, double n);
template float BinomialCoefficient::compute(float k, float n);

}

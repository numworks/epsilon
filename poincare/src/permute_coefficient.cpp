#include <poincare/permute_coefficient.h>
#include <poincare/undefined.h>
#include <poincare/complex.h>
#include <poincare/rational.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type PermuteCoefficient::type() const {
  return Type::PermuteCoefficient;
}

Expression * PermuteCoefficient::clone() const {
  PermuteCoefficient * b = new PermuteCoefficient(m_operands, true);
  return b;
}

Expression * PermuteCoefficient::shallowReduce(Context& context, AngleUnit angleUnit) {
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
    return replaceWith(new Rational(0), true);
  }
  if (Integer(k_maxNumberOfSteps).isLowerThan(k)) {
    return replaceWith(new Undefined(), true);
  }
  Integer result(1);
  int clippedK = k.extractedInt(); // Authorized because k < k_maxNumberOfSteps
  for (int i = 0; i < clippedK; i++) {
    Integer factor = Integer::Subtraction(n, Integer(i));
    result = Integer::Multiplication(result, factor);
  }
  return replaceWith(new Rational(result), true);
}

template<typename T>
Complex<T> * PermuteCoefficient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Complex<T> * nInput = operand(0)->privateEvaluate(T(), context, angleUnit);
  Complex<T> * kInput = operand(1)->privateEvaluate(T(), context, angleUnit);
  T n = nInput->toScalar();
  T k = kInput->toScalar();
  delete nInput;
  delete kInput;
  if (isnan(n) || isnan(k) || n != std::round(n) || k != std::round(k) || n < 0.0f || k < 0.0f || k > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (k > n) {
    return new Complex<T>(Complex<T>::Float(0));
  }
  T result = 1;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
  }
  return new Complex<T>(Complex<T>::Float(std::round(result)));
}

}


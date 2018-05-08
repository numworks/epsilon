#include <poincare/permute_coefficient.h>
#include <poincare/undefined.h>
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
    return replaceWith(new Rational(0), true);
  }
  /* if n is too big, we do not reduce to avoid too long computation.
   * The permute coefficient will be evaluate approximatively later */
  if (Integer(k_maxNValue).isLowerThan(n)) {
    return this;
  }
  Integer result(1);
  int clippedK = k.extractedInt(); // Authorized because k < n < k_maxNValue
  for (int i = 0; i < clippedK; i++) {
    Integer factor = Integer::Subtraction(n, Integer(i));
    result = Integer::Multiplication(result, factor);
  }
  return replaceWith(new Rational(result), true);
}

template<typename T>
Complex<T> * PermuteCoefficient::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * nInput = operand(0)->approximate<T>(context, angleUnit);
  Expression * kInput = operand(1)->approximate<T>(context, angleUnit);
  if (nInput->type() != Type::Complex || kInput->type() != Type::Complex) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T n = static_cast<Complex<T> *>(nInput)->toScalar();
  T k = static_cast<Complex<T> *>(kInput)->toScalar();
  delete nInput;
  delete kInput;
  if (std::isnan(n) || std::isnan(k) || n != std::round(n) || k != std::round(k) || n < 0.0f || k < 0.0f) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (k > n) {
    return new Complex<T>(Complex<T>::Float(0));
  }
  T result = 1;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
    if (std::isinf(result) || std::isnan(result)) {
      return new Complex<T>(Complex<T>::Float(result));
    }
  }
  return new Complex<T>(Complex<T>::Float(std::round(result)));
}

}


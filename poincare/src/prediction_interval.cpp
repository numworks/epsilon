#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/division.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type PredictionInterval::type() const {
  return Type::PredictionInterval;
}

Expression * PredictionInterval::clone() const {
  PredictionInterval * a = new PredictionInterval(m_operands, true);
  return a;
}

int PredictionInterval::polynomialDegree(char symbolName) const {
  return -1;
}

Expression * PredictionInterval::shallowReduce(Context& context, AngleUnit angleUnit) {
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
    if (r0->numerator().isNegative() || Integer::NaturalOrder(r0->numerator(), r0->denominator()) > 0) {
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
  if (!r1->denominator().isOne() || r1->numerator().isNegative() || r0->numerator().isNegative() || Integer::NaturalOrder(r0->numerator(), r0->denominator()) > 0) {
    return replaceWith(new Undefined(), true);
  }
  detachOperand(r0);
  detachOperand(r1);
  /* [r0-1.96*sqrt(r0*(1-r0)/r1), r0+1.96*sqrt(r0*(1-r0)/r1)]*/
  // Compute numerator = r0*(1-r0)
  Rational * numerator = new Rational(Rational::Multiplication(*r0, Rational(Integer::Subtraction(r0->denominator(), r0->numerator()), r0->denominator())));
  // Compute sqr = sqrt(r0*(1-r0)/r1)
  Expression * sqr = new Power(new Division(numerator, r1, false), new Rational(1, 2), false);
  Expression * m = new Multiplication(new Rational(196, 100), sqr, false);
  const Expression * newOperands[2] = {new Addition(r0, new Multiplication(new Rational(-1), m, false), false), new Addition(r0, m, true),};
  Expression * matrix = replaceWith(new Matrix(newOperands, 1, 2, false), true);
  return matrix->deepReduce(context, angleUnit);
}

template<typename T>
Expression * PredictionInterval::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * pInput = operand(0)->approximate<T>(context, angleUnit);
  Expression * nInput = operand(1)->approximate<T>(context, angleUnit);
  if (pInput->type() != Type::Complex || nInput->type() != Type::Complex) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T p = static_cast<Complex<T> *>(pInput)->toScalar();
  T n = static_cast<Complex<T> *>(nInput)->toScalar();
  delete pInput;
  delete nInput;
  if (std::isnan(p) || std::isnan(n) || n != (int)n || n < 0 || p < 0 || p > 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Expression * operands[2];
  operands[0] = new Complex<T>(Complex<T>::Float(p - 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n)));
  operands[1] = new Complex<T>(Complex<T>::Float(p + 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n)));
  return new Matrix(operands, 1, 2, false);
}

}


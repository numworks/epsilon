#include <poincare/confidence_interval.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ConfidenceInterval::type() const {
  return Type::ConfidenceInterval;
}

Expression * ConfidenceInterval::clone() const {
  ConfidenceInterval * a = new ConfidenceInterval(m_operands, true);
  return a;
}

int ConfidenceInterval::polynomialDegree(char symbolName) const {
  return -1;
}

Expression * ConfidenceInterval::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  detachOperands();
  // Compute [r0-1/sqr(r1), r0+1/sqr(r1)]
  Expression * sqr = new Power(r1, new Rational(-1, 2), false);
  const Expression * newOperands[2] = {new Addition(r0, new Multiplication(new Rational(-1), sqr, false), false), new Addition(r0, sqr, true)};
  Expression * matrix = replaceWith(new Matrix(newOperands, 1, 2, false), true);
  return matrix->deepReduce(context, angleUnit);
}

template<typename T>
Expression * ConfidenceInterval::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * fInput = operand(0)->approximate<T>(context, angleUnit);
  Expression * nInput = operand(1)->approximate<T>(context, angleUnit);
  if (fInput->type() != Type::Complex || nInput->type() != Type::Complex) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T f = static_cast<Complex<T> *>(fInput)->toScalar();
  T n = static_cast<Complex<T> *>(nInput)->toScalar();
  delete fInput;
  delete nInput;
  if (std::isnan(f) || std::isnan(n) || n != (int)n || n < 0 || f < 0 || f > 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Expression * operands[2];
  operands[0] = new Complex<T>(Complex<T>::Float(f - 1/std::sqrt(n)));
  operands[1] = new Complex<T>(Complex<T>::Float(f + 1/std::sqrt(n)));
  return new Matrix(operands, 1, 2, false);
}

}


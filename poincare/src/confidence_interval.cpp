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

Expression * ConfidenceInterval::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op0 = editableOperand(0);
  Expression * op1 = editableOperand(1);
  if (op0->type() != Type::Rational || op1->type() != Type::Rational) {
    return replaceWith(new Undefined(), true);
  }
  Rational * r0 = static_cast<Rational *>(op0);
  Rational * r1 = static_cast<Rational *>(op1);
  if (!r1->denominator().isOne() || r1->numerator().isNegative() || r0->numerator().isNegative() || Integer::NaturalOrder(r0->numerator(), r0->denominator()) <= 0) {
    return replaceWith(new Undefined(), true);
  }
  detachOperand(r0);
  detachOperand(r1);
  Expression * sqr = new Power(r1, new Rational(-1, 2), false);
  const Expression * newOperands[2] = {new Addition(r0, sqr, true),
                                       new Addition(r0, new Multiplication(new Rational(-1), sqr, false), false)};
  Expression * matrix = replaceWith(new Matrix(newOperands, 1, 2, false), true);
  return matrix->deepReduce(context, angleUnit);
}

}


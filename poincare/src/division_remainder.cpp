#include <poincare/division_remainder.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type DivisionRemainder::type() const {
  return Type::DivisionRemainder;
}

Expression * DivisionRemainder::clone() const {
  DivisionRemainder * a = new DivisionRemainder(m_operands, true);
  return a;
}

Expression * DivisionRemainder::shallowReduce(Context& context, AngleUnit angleUnit) {
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
    if (!r0->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
  }
  if (op1->type() == Type::Rational) {
    Rational * r1 = static_cast<Rational *>(op1);
    if (!r1->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
  }
  if (op0->type() != Type::Rational || op1->type() != Type::Rational) {
    return this;
  }
  Rational * r0 = static_cast<Rational *>(op0);
  Rational * r1 = static_cast<Rational *>(op1);

  Integer a = r0->numerator();
  Integer b = r1->numerator();
  if (b.isZero()) {
    return replaceWith(new Undefined(), true); // TODO: new Infinite(a.isNegative())
  }
  Integer result = Integer::Division(a, b).remainder;
  return replaceWith(new Rational(result), true);
}

template<typename T>
Complex<T> * DivisionRemainder::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * f1Input = operand(0)->approximate<T>(context, angleUnit);
  Expression * f2Input = operand(1)->approximate<T>(context, angleUnit);
  T f1 = f1Input->type() == Type::Complex ? static_cast<Complex<T> *>(f1Input)->toScalar() : NAN;
  T f2 = f2Input->type() == Type::Complex ? static_cast<Complex<T> *>(f2Input)->toScalar() : NAN;
  delete f1Input;
  delete f2Input;
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  return new Complex<T>(Complex<T>::Float(std::round(f1-f2*std::floor(f1/f2))));
}

}


#include <poincare/great_common_divisor.h>
#include <poincare/complex.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/arithmetic.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type GreatCommonDivisor::type() const {
  return Type::GreatCommonDivisor;
}

Expression * GreatCommonDivisor::clone() const {
  GreatCommonDivisor * a = new GreatCommonDivisor(m_operands, true);
  return a;
}

Expression * GreatCommonDivisor::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  Integer gcd = Arithmetic::GCD(&a, &b);
  return replaceWith(new Rational(gcd), true);
}

template<typename T>
Complex<T> * GreatCommonDivisor::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * f1Input = operand(0)->approximate<T>(context, angleUnit);
  Expression * f2Input = operand(1)->approximate<T>(context, angleUnit);
  T f1 = f1Input->type() == Type::Complex ? static_cast<Complex<T> *>(f1Input)->toScalar() : NAN;
  T f2 = f2Input->type() == Type::Complex ? static_cast<Complex<T> *>(f2Input)->toScalar() : NAN;
  delete f1Input;
  delete f2Input;
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return new Complex<T>(Complex<T>::Float(std::round((T)a)));
}

}


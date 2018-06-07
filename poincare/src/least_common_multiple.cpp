#include <poincare/least_common_multiple.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type LeastCommonMultiple::type() const {
  return Type::LeastCommonMultiple;
}

Expression * LeastCommonMultiple::clone() const {
  LeastCommonMultiple * a = new LeastCommonMultiple(m_operands, true);
  return a;
}

Expression * LeastCommonMultiple::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  Integer lcm = Arithmetic::LCM(&a, &b);
  return replaceWith(new Rational(lcm), true);
}

template<typename T>
Complex<T> * LeastCommonMultiple::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = operand(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * f2Input = operand(1)->privateApproximate(T(), context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Undefined());
  }
  if (f1 == 0.0f || f2 == 0.0f) {
    return new Complex<T>(0);
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int product = a*b;
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return new Complex<T>(product/a);
}

}


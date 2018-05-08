#include <poincare/nth_root.h>
#include <poincare/complex.h>
#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include "layout/nth_root_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type NthRoot::type() const {
  return Type::NthRoot;
}

Expression * NthRoot::clone() const {
  NthRoot * a = new NthRoot(m_operands, true);  return a;
}

Expression * NthRoot::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix || operand(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
#endif
  Power * invIndex = new Power(operand(1), new Rational(-1), false);
  Power * p = new Power(operand(0), invIndex, false);
  detachOperands();
  invIndex->shallowReduce(context, angleUnit);
  replaceWith(p, true);
  return p->shallowReduce(context, angleUnit);
}

ExpressionLayout * NthRoot::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(operand(0)->createLayout(floatDisplayMode, complexFormat), operand(1)->createLayout(floatDisplayMode, complexFormat));
}

template<typename T>
Complex<T> NthRoot::compute(const Complex<T> c, const Complex<T> d) {
  if (c.a() >= 0 && c.b() == 0 && d.b() == 0) {
    return Complex<T>::Float(std::pow(c.a(), 1/d.a()));
  }
  Complex<T> invIndex = Division::compute(Complex<T>::Float(1), d);
  return Power::compute(c, invIndex);
}

template<typename T>
Expression * NthRoot::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * base = operand(0)->approximate<T>(context, angleUnit);
  Expression * index = operand(1)->approximate<T>(context, angleUnit);
  Complex<T> result = Complex<T>::Float(NAN);
  if (base->type() == Type::Complex && index->type() == Type::Complex) {
    Complex<T> * basec = static_cast<Complex<T> *>(base);
    Complex<T> * indexc = static_cast<Complex<T> *>(index);
    result = compute(*basec, *indexc);
  }
  delete base;
  delete index;
  return new Complex<T>(result);
}

}

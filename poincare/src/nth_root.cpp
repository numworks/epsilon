#include <poincare/nth_root.h>
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

ExpressionLayout * NthRoot::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return new NthRootLayout(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), operand(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

template<typename T>
Evaluation<T> * NthRoot::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * base = operand(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * index = operand(1)->privateApproximate(T(), context, angleUnit);
  Complex<T> result = Complex<T>::Undefined();
  if (base->type() == Evaluation<T>::Type::Complex && index->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * basec = static_cast<Complex<T> *>(base);
    Complex<T> * indexc = static_cast<Complex<T> *>(index);
    result = std::pow(*basec, std::complex<T>(1)/(*indexc));
  }
  delete base;
  delete index;
  return new Complex<T>(result);
}

}

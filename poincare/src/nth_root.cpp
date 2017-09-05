#include <poincare/nth_root.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

NthRoot::NthRoot() :
  Function("root", 2)
{
}

Expression::Type NthRoot::type() const {
  return Type::NthRoot;
}

Expression * NthRoot::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  NthRoot * r = new NthRoot();
  r->setArgument(newOperands, numberOfOperands, cloneOperands);
  return r;
}

ExpressionLayout * NthRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat), m_args[1]->createLayout(floatDisplayMode, complexFormat));
}

template<typename T>
Evaluation<T> * NthRoot::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * base = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * index = m_args[1]->evaluate<T>(context, angleUnit);
  Complex<T> result = Complex<T>::Float(NAN);
  if (base->numberOfOperands() == 1 || index->numberOfOperands() == 1) {
    result = compute(*(base->complexOperand(0)), *(index->complexOperand(0)));
  }
  delete base;
  delete index;
  return new Complex<T>(result);
}

template<typename T>
Complex<T> NthRoot::compute(const Complex<T> c, const Complex<T> d) const {
  if (c.a() >= 0 && c.b() == 0 && d.b() == 0) {
    return Complex<T>::Float(std::pow(c.a(), 1/d.a()));
  }
  Complex<T> invIndex = Fraction::compute(Complex<T>::Float(1), d);
  return Power::compute(c, invIndex);
}

}

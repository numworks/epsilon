#include <poincare/nth_root.h>
#include <poincare/complex.h>
#include <poincare/division.h>
#include <poincare/power.h>
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

ExpressionLayout * NthRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
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
Evaluation<T> * NthRoot::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * base = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * index = operand(1)->evaluate<T>(context, angleUnit);
  Complex<T> result = Complex<T>::Float(NAN);
  if (base->numberOfOperands() == 1 || index->numberOfOperands() == 1) {
    result = compute(*(base->complexOperand(0)), *(index->complexOperand(0)));
  }
  delete base;
  delete index;
  return new Complex<T>(result);
}

}

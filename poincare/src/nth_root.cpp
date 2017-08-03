#include <poincare/nth_root.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"

extern "C" {
#include <assert.h>
#include <math.h>
}

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

Evaluation * NthRoot::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * base = m_args[0]->evaluate(context, angleUnit);
  Evaluation * index = m_args[1]->evaluate(context, angleUnit);
  Complex result = Complex::Float(NAN);
  if (base->numberOfOperands() == 1 || index->numberOfOperands() == 1) {
    result = compute(*(base->complexOperand(0)), *(index->complexOperand(0)));
  }
  delete base;
  delete index;
  return new Complex(result);
}

Complex NthRoot::compute(const Complex c, const Complex d) const {
  if (c.b() == 0.0f && d.b() == 0.0f) {
    return Complex::Float(powf(c.a(), 1.0f/d.a()));
  }
  Complex invIndex = Fraction::compute(Complex::Float(1.0f), d);
  return Power::compute(c, invIndex);
}

}

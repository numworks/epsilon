#include <poincare/square_root.h>
#include <poincare/complex.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

SquareRoot::SquareRoot() :
  Function("squareRoot")
{
}

Expression::Type SquareRoot::type() const {
  return Type::SquareRoot;
}

Expression * SquareRoot::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  SquareRoot * sr = new SquareRoot();
  sr->setArgument(newOperands, numberOfOperands, cloneOperands);
  return sr;
}

ExpressionLayout * SquareRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat),nullptr);
}

Complex SquareRoot::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() == 0.0f) {
    return Complex::Float(std::sqrt(c.a()));
  }
  return Power::compute(c, Complex::Float(0.5f));
}

}

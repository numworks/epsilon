#include <poincare/square_root.h>
#include <poincare/complex.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type SquareRoot::type() const {
  return Type::SquareRoot;
}

Expression * SquareRoot::clone() const {
  SquareRoot * a = new SquareRoot(m_operands, true);
  return a;
}

bool SquareRoot::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> SquareRoot::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() == 0 && c.a() >= 0) {
    return Complex<T>::Float(std::sqrt(c.a()));
  }
  return Power::compute(c, Complex<T>::Float(0.5));
}

ExpressionLayout * SquareRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(operand(0)->createLayout(floatDisplayMode, complexFormat),nullptr);
}

}

#include <poincare/conjugate.h>
#include <poincare/complex.h>
#include "layout/conjugate_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Conjugate::type() const {
  return Type::Conjugate;
}

Expression * Conjugate::clone() const {
  Conjugate * a = new Conjugate(m_operands, true);
  return a;
}

bool Conjugate::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> Conjugate::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return c.conjugate();
}

ExpressionLayout * Conjugate::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new ConjugateLayout(operand(0)->createLayout(floatDisplayMode, complexFormat));
}

}


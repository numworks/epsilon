#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>
#include <poincare/hyperbolic_tangent.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

Expression * Tangent::clone() const {
  Tangent * a = new Tangent(m_operands, true);
  return a;
}

Expression * Tangent::immediateSimplify(Context& context, AngleUnit angleUnit) {
  return Trigonometry::immediateSimplifyDirectFunction(this, context, angleUnit);
}

template<typename T>
Complex<T> Tangent::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  Complex<T> result = Division::compute(Sine::computeOnComplex(c, angleUnit), Cosine::computeOnComplex(c, angleUnit));
  if (!isnan(result.a()) && !isnan(result.b())) {
    return result;
  }
  Complex<T> tanh = HyperbolicTangent::computeOnComplex(Multiplication::compute(Complex<T>::Cartesian(0, -1), c), angleUnit);
  return Multiplication::compute(Complex<T>::Cartesian(0, 1), tanh);
}

}

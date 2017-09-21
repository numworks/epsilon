#include <poincare/hyperbolic_tangent.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicTangent::type() const {
  return Type::HyperbolicTangent;
}

Expression * HyperbolicTangent::clone() const {
  HyperbolicTangent * a = new HyperbolicTangent(m_operands, true);
  return a;
}

bool HyperbolicTangent::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> HyperbolicTangent::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() == 0) {
    return Complex<T>::Float(std::tanh(c.a()));
  }
  Complex<T> arg1 = HyperbolicSine::computeOnComplex(c, angleUnit);
  Complex<T> arg2 = HyperbolicCosine::computeOnComplex(c, angleUnit);
  return Fraction::compute(arg1, arg2);
}

}

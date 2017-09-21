#include <poincare/hyperbolic_arc_tangent.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcTangent::type() const {
  return Type::HyperbolicArcTangent;
}

Expression * HyperbolicArcTangent::clone() const {
  HyperbolicArcTangent * a = new HyperbolicArcTangent(m_operands, true);
  return a;
}

bool HyperbolicArcTangent::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> HyperbolicArcTangent::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::atanh(c.a()));
}

}

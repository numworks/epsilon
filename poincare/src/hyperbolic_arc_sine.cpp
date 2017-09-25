#include <poincare/hyperbolic_arc_sine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcSine::type() const {
  return Type::HyperbolicArcSine;
}

Expression * HyperbolicArcSine::clone() const {
  HyperbolicArcSine * a = new HyperbolicArcSine(m_operands, true);
  return a;
}

template<typename T>
Complex<T> HyperbolicArcSine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::asinh(c.a()));
}

}

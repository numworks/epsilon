#include <poincare/hyperbolic_arc_cosine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcCosine::type() const {
  return Type::HyperbolicArcCosine;
}

Expression * HyperbolicArcCosine::clone() const {
  HyperbolicArcCosine * a = new HyperbolicArcCosine(m_operands, true);
  return a;
}

bool HyperbolicArcCosine::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> HyperbolicArcCosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::acosh(c.a()));
}

}

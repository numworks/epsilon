#include <poincare/ceiling.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Ceiling::type() const {
  return Type::Ceiling;
}

Expression * Ceiling::clone() const {
  Ceiling * c = new Ceiling(m_operands, true);
  return c;
}

bool Ceiling::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> Ceiling::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::ceil(c.a()));
}

}



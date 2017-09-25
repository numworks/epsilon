#include <poincare/floor.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Floor::type() const {
  return Type::Floor;
}

Expression * Floor::clone() const {
  Floor * c = new Floor(m_operands, true);
  return c;
}

template<typename T>
Complex<T> Floor::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::floor(c.a()));
}

}



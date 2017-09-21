#include <poincare/naperian_logarithm.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Expression::Type NaperianLogarithm::type() const {
  return Type::NaperianLogarithm;
}

Expression * NaperianLogarithm::clone() const {
  NaperianLogarithm * a = new NaperianLogarithm(m_operands, true);
  return a;
}

bool NaperianLogarithm::isCommutative() const {
  return false;
}

template<typename T>
Complex<T> NaperianLogarithm::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log(c.a()));
}

}

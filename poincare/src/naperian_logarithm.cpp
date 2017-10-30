#include <poincare/naperian_logarithm.h>
#include <poincare/symbol.h>
#include <poincare/logarithm.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <ion.h>
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

Expression * NaperianLogarithm::shallowSimplify(Context& context, AngleUnit angleUnit) {
  const Expression * logOperands[2] = {operand(0)->clone(), new Symbol(Ion::Charset::Exponential)};
  Logarithm * l = new Logarithm(logOperands, 2, false);
  replaceWith(l, true);
  return l->shallowSimplify(context, angleUnit);
}

template<typename T>
Complex<T> NaperianLogarithm::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log(c.a()));
}

}

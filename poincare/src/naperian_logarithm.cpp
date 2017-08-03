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

NaperianLogarithm::NaperianLogarithm() :
  Function("ln")
{
}

Expression::Type NaperianLogarithm::type() const {
  return Type::NaperianLogarithm;
}

Expression * NaperianLogarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  NaperianLogarithm * l = new NaperianLogarithm();
  l->setArgument(newOperands, numberOfOperands, cloneOperands);
  return l;
}

template<typename T>
Complex<T> NaperianLogarithm::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log(c.a()));
}

}

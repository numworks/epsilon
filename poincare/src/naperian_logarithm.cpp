#include <poincare/naperian_logarithm.h>
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}
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

Complex NaperianLogarithm::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(logf(c.a()));
}

}

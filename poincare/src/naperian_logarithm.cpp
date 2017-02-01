#include <poincare/naperian_logarithm.h>
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

NaperianLogarithm::NaperianLogarithm() :
  Function("ln")
{
}

Expression::Type NaperianLogarithm::type() const {
  return Type::NaperianLogarithm;
}

Expression * NaperianLogarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1 || numberOfOperands == 2);
  assert(newOperands != nullptr);
  NaperianLogarithm * l = new NaperianLogarithm();
  l->setArgument(newOperands, numberOfOperands, cloneOperands);
  return l;
}

float NaperianLogarithm::approximate(Context& context, AngleUnit angleUnit) const {
  return log10f(m_args[0]->approximate(context, angleUnit))/log10f(M_E);
}

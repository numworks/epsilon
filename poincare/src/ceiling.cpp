#include <poincare/ceiling.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Ceiling::Ceiling() :
  Function("ceil")
{
}

Expression::Type Ceiling::type() const {
  return Type::Ceiling;
}

Expression * Ceiling::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Ceiling * c = new Ceiling();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float Ceiling::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f = m_args[0]->approximate(context, angleUnit);
  return ceilf(f);
}

}



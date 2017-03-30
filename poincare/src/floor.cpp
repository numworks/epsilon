#include <poincare/floor.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Floor::Floor() :
  Function("floor")
{
}

Expression::Type Floor::type() const {
  return Type::Floor;
}

Expression * Floor::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Floor * f = new Floor();
  f->setArgument(newOperands, numberOfOperands, cloneOperands);
  return f;
}

float Floor::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f = m_args[0]->approximate(context, angleUnit);
  return floorf(f);
}

}



#include <poincare/reel_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ReelPart::ReelPart() :
  Function("re")
{
}

Expression::Type ReelPart::type() const {
  return Type::ReelPart;
}

Expression * ReelPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ReelPart * rp = new ReelPart();
  rp->setArgument(newOperands, numberOfOperands, cloneOperands);
  return rp;
}

float ReelPart::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  float result = 0.0f;
  if (evaluation->type() == Type::Matrix) {
    result = NAN;
  } else {
    result = ((Complex *)evaluation)->a();
  }
  delete evaluation;
  return result;
}

}



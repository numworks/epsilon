#include <poincare/imaginary_part.h>
#include <poincare/complex.h>

extern "C" {
#include <math.h>
#include <assert.h>
}

namespace Poincare {

ImaginaryPart::ImaginaryPart() :
  Function("im")
{
}

Expression::Type ImaginaryPart::type() const {
  return Type::ImaginaryPart;
}

Expression * ImaginaryPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ImaginaryPart * ip = new ImaginaryPart();
  ip->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ip;
}

float ImaginaryPart::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  float result = 0.0f;
  if (evaluation->type() == Type::Matrix) {
    result = NAN;
  } else {
    result = ((Complex *)evaluation)->b();
  }
  delete evaluation;
  return result;
}

}



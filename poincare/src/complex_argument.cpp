#include <poincare/complex_argument.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ComplexArgument::ComplexArgument() :
  Function("arg")
{
}

Expression::Type ComplexArgument::type() const {
  return Type::ComplexArgument;
}

Expression * ComplexArgument::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ComplexArgument * ca = new ComplexArgument();
  ca->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ca;
}

float ComplexArgument::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  float result = 0.0f;
  if (evaluation->type() == Type::Matrix) {
    result = NAN;
  } else {
    result = ((Complex *)evaluation)->th();
  }
  delete evaluation;
  return result;
}

}


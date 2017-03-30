#include <poincare/determinant.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Determinant::Determinant() :
  Function("det")
{
}

Expression::Type Determinant::type() const {
  return Type::Determinant;
}

Expression * Determinant::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Determinant * d = new Determinant();
  d->setArgument(newOperands, numberOfOperands, cloneOperands);
  return d;
}

float Determinant::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Complex) {
    float result = evaluation->approximate(context, angleUnit);
    delete evaluation;
    return result;
  }
  float det = ((Matrix *)evaluation)->determinant(context, angleUnit);
  delete evaluation;
  return det;
}

}


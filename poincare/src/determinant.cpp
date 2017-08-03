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

Evaluation * Determinant::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * result = input->createDeterminant();
  delete input;
  return result;
}

}


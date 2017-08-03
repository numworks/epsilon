#include <poincare/determinant.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

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

template<typename T>
Evaluation<T> * Determinant::templatedEvaluate(Context& context, AngleUnit angleUnit) const {

  Evaluation<T> * input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createDeterminant();
  delete input;
  return result;
}

}


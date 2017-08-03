#include <poincare/matrix_inverse.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

MatrixInverse::MatrixInverse() :
  Function("inverse")
{
}

Expression::Type MatrixInverse::type() const {
  return Type::MatrixInverse;
}

Expression * MatrixInverse::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixInverse * i = new MatrixInverse();
  i->setArgument(newOperands, numberOfOperands, cloneOperands);
  return i;
}

Evaluation * MatrixInverse::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * result = input->createInverse();
  delete input;
  return result;
}

}


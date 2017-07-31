#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

MatrixTranspose::MatrixTranspose() :
  Function("transpose")
{
}

Expression::Type MatrixTranspose::type() const {
  return Type::MatrixTranspose;
}

Expression * MatrixTranspose::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixTranspose * t = new MatrixTranspose();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Evaluation * MatrixTranspose::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * result = input->createTranspose();
  delete input;
  return result;
}

}


#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

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

template<typename T>
Evaluation<T> * MatrixTranspose::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createTranspose();
  delete input;
  return result;
}

}


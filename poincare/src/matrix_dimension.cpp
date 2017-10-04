#include <poincare/matrix_dimension.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

MatrixDimension::MatrixDimension() :
  Function("dimension")
{
}

Expression::Type MatrixDimension::type() const {
  return Type::MatrixDimension;
}

Expression * MatrixDimension::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixDimension * md = new MatrixDimension();
  md->setArgument(newOperands, numberOfOperands, cloneOperands);
  return md;
}

template<typename T>
Evaluation<T> * MatrixDimension::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = m_args[0]->evaluate<T>(context, angleUnit);
  Complex<T> operands[2];
  operands[0] = Complex<T>::Float((T)input->numberOfRows());
  operands[1] = Complex<T>::Float((T)input->numberOfColumns());
  delete input;
  return new ComplexMatrix<T>(operands, 1, 2);
}

}


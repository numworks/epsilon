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

Evaluation * MatrixDimension::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Complex operands[2];
  operands[0] = Complex::Float((float)input->numberOfRows());
  operands[1] = Complex::Float((float)input->numberOfColumns());
  delete input;
  return new ComplexMatrix(operands, 2, 1);
}

}


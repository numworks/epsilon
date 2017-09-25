#include <poincare/matrix_dimension.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type MatrixDimension::type() const {
  return Type::MatrixDimension;
}

Expression * MatrixDimension::clone() const {
  MatrixDimension * a = new MatrixDimension(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * MatrixDimension::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = operand(0)->evaluate<T>(context, angleUnit);
  Complex<T> operands[2];
  operands[0] = Complex<T>::Float((T)input->numberOfRows());
  operands[1] = Complex<T>::Float((T)input->numberOfColumns());
  delete input;
  return new ComplexMatrix<T>(operands, 2, 1);
}

}


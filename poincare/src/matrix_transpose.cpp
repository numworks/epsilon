#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/division.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type MatrixTranspose::type() const {
  return Type::MatrixTranspose;
}

Expression * MatrixTranspose::clone() const {
  MatrixTranspose * a = new MatrixTranspose(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * MatrixTranspose::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createTranspose();
  delete input;
  return result;
}

}


#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
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

Expression * MatrixTranspose::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    Matrix * transpose = static_cast<Matrix *>(op)->createTranspose();
    return replaceWith(transpose, true);
  }
  if (!op->recursivelyMatches(Expression::IsMatrix)) {
    return replaceWith(op, true);
  }
  return this;
#else
  return replaceWith(op, true);
#endif
}

template<typename T>
Evaluation<T> * MatrixTranspose::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = operand(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * result = nullptr;
  if (input->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(input);
    result = new Complex<T>(*c);
  } else {
    assert(input->type() == Evaluation<T>::Type::MatrixComplex);
    result = new MatrixComplex<T>(static_cast<MatrixComplex<T> *>(input)->createTranspose());
  }
  assert(result != nullptr);
  delete input;
  return result;
}

}

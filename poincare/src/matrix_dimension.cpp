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

Expression * MatrixDimension::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    Matrix * m = static_cast<Matrix *>(op);
    const Expression * newOperands[2] = {new Rational(m->numberOfRows()), new Rational(m->numberOfColumns())};
    return replaceWith(new Matrix(newOperands, 1, 2, false), true);
  }
  if (!op->recursivelyMatches(Expression::IsMatrix)) {
    const Expression * newOperands[2] = {new Rational(1), new Rational(1)};
    return replaceWith(new Matrix(newOperands, 1, 2, false), true);
  }
  return this;
#else
  const Expression * newOperands[2] = {new Rational(1), new Rational(1)};
  return replaceWith(new Matrix(newOperands, 1, 2, false), true);
#endif
}

template<typename T>
Expression * MatrixDimension::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * input = operand(0)->approximate<T>(context, angleUnit);
  Expression * operands[2];
  if (input->type() == Type::Matrix) {
    operands[0] = new Complex<T>(Complex<T>::Float((T)static_cast<Matrix *>(input)->numberOfRows()));
    operands[1] = new Complex<T>(Complex<T>::Float((T)static_cast<Matrix *>(input)->numberOfColumns()));
  } else {
    operands[0] = new Complex<T>(Complex<T>::Float(1.0));
    operands[1] = new Complex<T>(Complex<T>::Float(1.0));
  }
  delete input;
  return new Matrix(operands, 1, 2, false);
}


}


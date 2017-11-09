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
  if (operand(0)->type() != Type::Matrix) {
    const Expression * newOperands[2] = {new Rational(1), new Rational(1)};
    return replaceWith(new Matrix(newOperands, 1, 2, false), true);
  }
  Matrix * m = static_cast<Matrix *>(editableOperand(0));
  const Expression * newOperands[2] = {new Rational(m->numberOfRows()), new Rational(m->numberOfColumns())};
  return replaceWith(new Matrix(newOperands, 1, 2, false), true);
}

}


#include <poincare/matrix_inverse.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/division.h>
#include <poincare/power.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type MatrixInverse::type() const {
  return Type::MatrixInverse;
}

Expression * MatrixInverse::clone() const {
  MatrixInverse * a = new MatrixInverse(m_operands, true);
  return a;
}

Expression * MatrixInverse::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    // TODO: handle this exactly ; now, we approximate the operand and compute the inverse matrix on real only.
    Expression * approxMatrix = op->evaluate<double>(context, angleUnit);
    assert(approxMatrix->type() == Type::Matrix);
    Expression * inverse = static_cast<Matrix *>(approxMatrix)->createInverse<double>();
    delete approxMatrix;
    return replaceWith(inverse, true);
  }
  detachOperand(op);
  return replaceWith(new Power(op, new Rational(-1), false), true)->shallowReduce(context, angleUnit);
}

}


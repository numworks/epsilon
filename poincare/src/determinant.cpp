#include <poincare/determinant.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Determinant::type() const {
  return Type::Determinant;
}

Expression * Determinant::clone() const {
  Determinant * a = new Determinant(m_operands, true);
  return a;
}

Expression * Determinant::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    // TODO: handle this exactly ; now, we approximate the operand and compute the determinant on real only.
    Expression * approxMatrix = op->evaluate<double>(context, angleUnit);
    assert(approxMatrix->type() == Type::Matrix);
    Complex<double> * det = static_cast<Matrix *>(approxMatrix)->createDeterminant<double>();
    delete approxMatrix;
    return replaceWith(det, true);
  }
  return replaceWith(op, true);
}

}


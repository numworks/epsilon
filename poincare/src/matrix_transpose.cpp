#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/division.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ExpressionNode::Type MatrixTranspose::type() const {
  return Type::MatrixTranspose;
}

Expression * MatrixTranspose::clone() const {
  MatrixTranspose * a = new MatrixTranspose(m_operands, true);
  return a;
}

Expression MatrixTranspose::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression * op = childAtIndex(0);
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
Evaluation<T> MatrixTranspose::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> * input = childAtIndex(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * transpose = input->createTranspose();
  assert(transpose != nullptr);
  delete input;
  return transpose;
}

}

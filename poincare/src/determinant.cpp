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
#if MATRIX_EXACT_REDUCING
  if (!op->recursivelyMatches(Expression::IsMatrix)) {
    return replaceWith(op, true);
  }
  return this;
#else
  return replaceWith(op, true);
#endif
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> * Determinant::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = operand(0)->privateApproximate(T(), context, angleUnit);
  Complex<T> * result = new Complex<T>(input->createDeterminant());
  delete input;
  return result;
}

}


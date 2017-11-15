#include <poincare/matrix_trace.h>
#include <poincare/matrix.h>
#include <poincare/undefined.h>
#include <poincare/addition.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type MatrixTrace::type() const {
  return Type::MatrixTrace;
}

Expression * MatrixTrace::clone() const {
  MatrixTrace * a = new MatrixTrace(m_operands, true);
  return a;
}

Expression * MatrixTrace::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    Matrix * m = static_cast<Matrix *>(op);
    if (m->numberOfRows() != m->numberOfColumns()) {
      return replaceWith(new Undefined(), true);
    }
    int n = m->numberOfRows();
    Addition * a = new Addition();
    for (int i = 0; i < n; i++) {
      Expression * diagEntry = m->editableOperand(i+n*i);
      m->detachOperand(diagEntry);
      a->addOperand(diagEntry);
    }
    return replaceWith(a, true)->shallowReduce(context, angleUnit);
  }
  return replaceWith(op, true);
}

}


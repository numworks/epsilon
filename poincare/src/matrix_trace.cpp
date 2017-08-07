#include <poincare/matrix_trace.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

MatrixTrace::MatrixTrace() :
  Function("trace")
{
}

Expression::Type MatrixTrace::type() const {
  return Type::MatrixTrace;
}

Expression * MatrixTrace::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixTrace * t = new MatrixTrace();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Evaluation * MatrixTrace::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * result = input->createTrace();
  delete input;
  return result;
}

}


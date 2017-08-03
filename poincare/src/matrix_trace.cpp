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

template<typename T>
Evaluation<T> * MatrixTrace::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createTrace();
  delete input;
  return result;
}

}


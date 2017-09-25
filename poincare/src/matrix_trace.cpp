#include <poincare/matrix_trace.h>
#include <poincare/matrix.h>
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

template<typename T>
Evaluation<T> * MatrixTrace::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * input = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = input->createTrace();
  delete input;
  return result;
}

}


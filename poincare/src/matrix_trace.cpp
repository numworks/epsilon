#include <poincare/matrix_trace.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

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
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  MatrixTrace * t = new MatrixTrace();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

float MatrixTrace::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Complex) {
    float result = evaluation->approximate(context, angleUnit);
    delete evaluation;
    return result;
  }
  float trace = ((Matrix *)evaluation)->trace(context, angleUnit);
  delete evaluation;
  return trace;
}

}


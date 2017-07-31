#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

MatrixTranspose::MatrixTranspose() :
  Function("transpose")
{
}

Expression::Type MatrixTranspose::type() const {
  return Type::MatrixTranspose;
}

Expression * MatrixTranspose::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixTranspose * t = new MatrixTranspose();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

float MatrixTranspose::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * MatrixTranspose::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Complex) {
    return evaluation;
  }
  Expression * transpose = ((Matrix *)evaluation)->createTranspose(context, angleUnit);
  delete evaluation;
  return transpose;
}

}


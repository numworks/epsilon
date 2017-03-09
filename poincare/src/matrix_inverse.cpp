#include <poincare/matrix_inverse.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

MatrixInverse::MatrixInverse() :
  Function("inverse")
{
}

Expression::Type MatrixInverse::type() const {
  return Type::MatrixInverse;
}

Expression * MatrixInverse::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  MatrixInverse * i = new MatrixInverse();
  i->setArgument(newOperands, numberOfOperands, cloneOperands);
  return i;
}

float MatrixInverse::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * MatrixInverse::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Complex) {
    Expression * arguments[2];
    arguments[0] = new Complex(Complex::Float(2.0f));
    arguments[1] = evaluation;
    Expression * result = new Fraction(arguments, true);
    delete arguments[0];
    delete arguments[1];
    Expression * resultEvaluation = result->evaluate(context, angleUnit);
    delete result;
    return resultEvaluation;
  }
  Expression * inverse = ((Matrix *)evaluation)->createInverse(context, angleUnit);
  delete evaluation;
  return inverse;
}

}


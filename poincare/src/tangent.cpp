#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Tangent::Tangent() :
  Function("tan")
{
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  Tangent * t = new Tangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return tanf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return tanf(m_args[0]->approximate(context, angleUnit));
}

Expression * Tangent::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  Expression * arguments[2];
  arguments[0] = new Sine();
  ((Function *)arguments[0])->setArgument(&evaluation, 1, true);
  arguments[1] = new Cosine();
  ((Function *)arguments[1])->setArgument(&evaluation, 1, true);
  delete evaluation;
  Expression * result = new Fraction(arguments, true);
  delete arguments[1];
  delete arguments[0];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

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
  TrigonometricFunction("tan")
{
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Tangent * t = new Tangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::trigonometricApproximation(float x) const {
  return tanf(x);
}

Expression * Tangent::createComplexEvaluation(Expression * exp, Context & context, AngleUnit angleUnit) const {
  assert(exp->type() == Type::Complex);
  Expression * arguments[2];
  arguments[0] = new Sine();
  ((Function *)arguments[0])->setArgument(&exp, 1, true);
  arguments[1] = new Cosine();
  ((Function *)arguments[1])->setArgument(&exp, 1, true);
  Expression * result = new Fraction(arguments, true);
  delete arguments[1];
  delete arguments[0];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

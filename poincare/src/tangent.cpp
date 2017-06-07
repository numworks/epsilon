#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include <poincare/hyperbolic_tangent.h>
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
  Expression * result = new Fraction(arguments, false);
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  if (!isnan(((Complex *)resultEvaluation)->a()) && !isnan(((Complex *)resultEvaluation)->b())) {
    return resultEvaluation;
  }
  delete resultEvaluation;
  arguments[0] = new Complex(Complex::Cartesian(0.0f, -1.0f));
  arguments[1] = exp;
  Expression * arg = new Multiplication(arguments, true);
  delete arguments[0];
  arguments[0] = new Complex(Complex::Cartesian(0.0f, 1.0f));
  arguments[1] = new HyperbolicTangent();
  ((Function *)arguments[1])->setArgument(&arg, 1, false);
  result = new Multiplication(arguments, false);
  resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

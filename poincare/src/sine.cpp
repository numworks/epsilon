#include <poincare/sine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/multiplication.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Sine::Sine() :
  TrigonometricFunction("sin")
{
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

Expression * Sine::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Sine * s = new Sine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

float Sine::trigonometricApproximation(float x) const {
  return sinf(x);
}

Expression * Sine::createComplexEvaluation(Expression * exp, Context & context, AngleUnit angleUnit) const {
  assert(exp->type() == Type::Complex);
  Expression * arg = new Complex(Complex::Cartesian(-((Complex *)exp)->b(), ((Complex *)exp)->a()));
  Function * sinh = new HyperbolicSine();
  sinh->setArgument(&arg, 1, true);
  delete arg;
  Expression * args[2];
  args[0] = new Complex(Complex::Cartesian(0.0f, -1.0f));
  args[1] = sinh;
  Multiplication * result = new Multiplication(args, true);
  delete args[0];
  delete args[1];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

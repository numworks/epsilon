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
  Function("sin")
{
}

Expression * Sine::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  Sine * s = new Sine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

float Sine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return sinf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return sinf(m_args[0]->approximate(context, angleUnit));
}

Expression * Sine::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(NAN);
  }
  Expression * arg = new Complex(-((Complex *)evaluation)->b(), ((Complex *)evaluation)->a());
  Function * sinh = new HyperbolicSine();
  sinh->setArgument(&arg, 1, true);
  delete evaluation;
  delete arg;
  Expression * args[2];
  args[0] = new Complex(0.0f, -1.0f);
  args[1] = sinh;
  Multiplication * result = new Multiplication(args, true);
  delete args[0];
  delete args[1];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

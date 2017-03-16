#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/fraction.h>
#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicSine::HyperbolicSine() :
  Function("sinh")
{
}

Expression::Type HyperbolicSine::type() const {
  return Type::HyperbolicSine;
}

Expression * HyperbolicSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  HyperbolicSine * hs = new HyperbolicSine();
  hs->setArgument(newOperands, numberOfOperands, cloneOperands);
  return hs;
}

float HyperbolicSine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return sinhf(m_args[0]->approximate(context, angleUnit));
}

Expression * HyperbolicSine::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  /* Float case */
  if (((Complex *)evaluation)->b() == 0) {
    delete evaluation;
    return Function::privateEvaluate(context, angleUnit);
  }
  /* Complex case */
  Expression * arguments[2];
  arguments[0] = new Complex(Complex::Float(M_E));
  arguments[1] = evaluation;
  Expression * exp1 = new Power(arguments, true);
  arguments[1] = new Opposite(evaluation, true);
  Expression * exp2 = new Power(arguments, true);
  delete arguments[1];
  delete arguments[0];
  delete evaluation;
  arguments[0] = exp1;
  arguments[1] = exp2;
  Expression * sub = new Subtraction(arguments, true);
  delete exp1;
  delete exp2;
  arguments[0] = sub;
  arguments[1] = new Complex(Complex::Float(2.0f));
  Expression * result = new Fraction(arguments, true);
  delete arguments[1];
  delete arguments[0];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

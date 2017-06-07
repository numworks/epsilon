#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
#include <poincare/power.h>
#include <poincare/fraction.h>
#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicCosine::HyperbolicCosine() :
  Function("cosh")
{
}

Expression::Type HyperbolicCosine::type() const {
  return Type::HyperbolicCosine;
}

Expression * HyperbolicCosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicCosine * hc = new HyperbolicCosine();
  hc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return hc;
}

float HyperbolicCosine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return coshf(m_args[0]->approximate(context, angleUnit));
}

Expression * HyperbolicCosine::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  /* Float case */
  if (((Complex *)evaluation)->b() == 0) {
    Expression * result = new Complex(Complex::Float(coshf(evaluation->approximate(context, angleUnit))));
    delete evaluation;
    return result;
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
  Expression * sum = new Addition(arguments, true);
  delete exp1;
  delete exp2;
  arguments[0] = sum;
  arguments[1] = new Complex(Complex::Float(2.0f));
  Expression * result = new Fraction(arguments, true);
  delete arguments[1];
  delete arguments[0];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

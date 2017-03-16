#include <poincare/hyperbolic_tangent.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicTangent::HyperbolicTangent() :
  Function("tanh")
{
}

Expression::Type HyperbolicTangent::type() const {
  return Type::HyperbolicTangent;
}

Expression * HyperbolicTangent::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  HyperbolicTangent * ht = new HyperbolicTangent();
  ht->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ht;
}

float HyperbolicTangent::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return tanhf(m_args[0]->approximate(context, angleUnit));
}

Expression * HyperbolicTangent::privateEvaluate(Context& context, AngleUnit angleUnit) const {
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
  arguments[0] = new HyperbolicSine();
  ((Function *)arguments[0])->setArgument(&evaluation, 1, true);
  arguments[1] = new HyperbolicCosine();
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

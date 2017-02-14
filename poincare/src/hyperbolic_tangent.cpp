#include <poincare/hyperbolic_tangent.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

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

float HyperbolicTangent::approximate(Context& context, AngleUnit angleUnit) const {
  return (expf(m_args[0]->approximate(context, angleUnit))-expf(-m_args[0]->approximate(context, angleUnit)))/
    (expf(m_args[0]->approximate(context, angleUnit))+expf(-m_args[0]->approximate(context, angleUnit)));
}

Expression * HyperbolicTangent::evaluate(Context& context, AngleUnit angleUnit) const {
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(NAN);
  }
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

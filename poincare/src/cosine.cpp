#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

Cosine::Cosine() :
  Function("cos")
{
}

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float Cosine::approximate(Context& context, AngleUnit angleUnit) const {
  if (angleUnit == AngleUnit::Degree) {
    return cosf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return cosf(m_args[0]->approximate(context, angleUnit));
}

Expression * Cosine::evaluate(Context& context, AngleUnit angleUnit) const {
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(NAN);
  }
  Expression * arg = new Complex(-((Complex *)evaluation)->b(), ((Complex *)evaluation)->a());
  Function * cosh = new HyperbolicCosine();
  cosh->setArgument(&arg, 1, true);
  delete evaluation;
  delete arg;
  Expression * resultEvaluation = cosh->evaluate(context, angleUnit);
  delete cosh;
  return resultEvaluation;
}

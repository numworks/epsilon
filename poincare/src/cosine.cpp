#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

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

float Cosine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return cosf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return cosf(m_args[0]->approximate(context, angleUnit));
}

Expression * Cosine::privateEvaluate(Context& context, AngleUnit angleUnit) const {
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
  Expression * arg = new Complex(Complex::Cartesian(-((Complex *)evaluation)->b(), ((Complex *)evaluation)->a()));
  Function * cosh = new HyperbolicCosine();
  cosh->setArgument(&arg, 1, true);
  delete evaluation;
  delete arg;
  Expression * resultEvaluation = cosh->evaluate(context, angleUnit);
  delete cosh;
  return resultEvaluation;
}

}

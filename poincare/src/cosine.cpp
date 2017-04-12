#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Cosine::Cosine() :
  TrigonometricFunction("cos")
{
}

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float Cosine::trigonometricApproximation(float x) const {
  return cosf(x);
}

Expression * Cosine::createComplexEvaluation(Expression * exp, Context & context, AngleUnit angleUnit) const {
  assert(exp->type() == Type::Complex);
  Expression * arg = new Complex(Complex::Cartesian(-((Complex *)exp)->b(), ((Complex *)exp)->a()));
  Function * cosh = new HyperbolicCosine();
  cosh->setArgument(&arg, 1, true);
  delete arg;
  Expression * resultEvaluation = cosh->evaluate(context, angleUnit);
  delete cosh;
  return resultEvaluation;
}

}

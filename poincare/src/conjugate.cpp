#include <poincare/conjugate.h>
#include <poincare/complex.h>
#include "layout/conjugate_layout.h"

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Conjugate::Conjugate() :
  Function("conj")
{
}

Expression::Type Conjugate::type() const {
  return Type::Conjugate;
}

Expression * Conjugate::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Conjugate * c = new Conjugate();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float Conjugate::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return m_args[0]->approximate(context, angleUnit);
}

Expression * Conjugate::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  Expression * result = ((Complex *)evaluation)->createConjugate();
  delete evaluation;
  return result;
}

ExpressionLayout * Conjugate::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new ConjugateLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

}


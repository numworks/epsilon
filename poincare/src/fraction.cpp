extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, cloneOperands);
}

ExpressionLayout * Fraction::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new FractionLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat), m_operands[1]->createLayout(floatDisplayMode, complexFormat));
}

float Fraction::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return m_operands[0]->approximate(context, angleUnit)/m_operands[1]->approximate(context, angleUnit);
}

Expression::Type Fraction::type() const {
  return Type::Fraction;
}

Expression * Fraction::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  float norm = d->a()*d->a() + d->b()*d->b();
  return new Complex(Complex::Cartesian((c->a()*d->a()+c->b()*d->b())/norm, (d->a()*c->b()-c->a()*d->b())/norm));
}

Expression * Fraction::evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const {
  return new Complex(Complex::Float(NAN));
}

Expression * Fraction::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return new Complex(Complex::Float(NAN));
  }
  if (fabsf(n->determinant(context, angleUnit)) <= FLT_EPSILON) {
    return new Complex(Complex::Float(NAN));
  }
  Expression * args[2];
  args[0] = m;
  args[1] = n->createInverse(context, angleUnit);
  Expression * result = new Multiplication(args, true);
  delete args[1];
  Expression * resultEvaluation = result->evaluate(context, angleUnit);
  delete result;
  return resultEvaluation;
}

}

extern "C" {
#include <assert.h>
#include <string.h>
}

#include <poincare/fraction.h>
#include "layout/fraction_layout.h"

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, cloneOperands);
}

ExpressionLayout * Fraction::createLayout(DisplayMode displayMode) const {
  return new FractionLayout(m_operands[0]->createLayout(displayMode), m_operands[1]->createLayout(displayMode));
}

float Fraction::approximate(Context& context, AngleUnit angleUnit) const {
  return m_operands[0]->approximate(context, angleUnit)/m_operands[1]->approximate(context, angleUnit);
}

Expression::Type Fraction::type() const {
  return Type::Fraction;
}

Expression * Fraction::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  float norm = d->a()*d->a() + d->b()*d->b();
  return new Complex((c->a()*d->a()+c->b()*d->b())/norm, (d->a()*c->b()-c->a()*d->b())/norm);
}

Expression * Fraction::evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const {
  return nullptr;
}

Expression * Fraction::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  /* TODO: implement matrix fraction
  if (n->det() == 0) {
    return new Complex(NAN);
  }
  result = new Product(m, n->inv(), false);
  return result;*/
  return nullptr;
}

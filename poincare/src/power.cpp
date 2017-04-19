extern "C" {
#include <assert.h>
#include <math.h>
}
#include <poincare/power.h>
#include <poincare/multiplication.h>
#include "layout/baseline_relative_layout.h"

namespace Poincare {

float Power::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return powf(m_operands[0]->approximate(context, angleUnit), m_operands[1]->approximate(context, angleUnit));
}

Expression::Type Power::type() const {
  return Type::Power;
}

Expression * Power::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  return new Power(newOperands, cloneOperands);
}

ExpressionLayout * Power::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  Expression * indiceOperand = m_operands[1];
  // Delete eventual parentheses of the indice in the pretty print
  if (m_operands[1]->type() == Type::Parenthesis) {
    indiceOperand = (Expression *)m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat),indiceOperand->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Superscript);
}

Expression * Power::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  if (d->b() != 0.0f) {
    /* First case c and d is complex */
    if (c->b() != 0.0f || c->a() <= 0) {
      return new Complex(Complex::Float(NAN));
    }
    /* Second case only d is complex */
    float radius = powf(c->a(), d->a());
    float theta = d->b()*logf(c->a());
    return new Complex(Complex::Polar(radius, theta));
  }
  /* Third case only c is complex */
  float radius = powf(c->r(), d->a());
  if (c->b() == 0 && d->a() == roundf(d->a())) {
    /* We handle the case -c float and d integer- separatly to avoid getting
     * complex result due to float representation: a float power an integer is
     * always real. */
    return new Complex(Complex::Cartesian(radius, 0.0f));
  }
  /* Third case only c is complex */
  float theta = d->a()*c->th();
  return new Complex(Complex::Polar(radius, theta));
}

Expression * Power::evaluateOnMatrixAndComplex(Matrix * m, Complex * c, Context& context, AngleUnit angleUnit) const {
  if (isnan(m_operands[1]->approximate(context, angleUnit)) || m_operands[1]->approximate(context, angleUnit) != (int)m_operands[1]->approximate(context, angleUnit)) {
    return new Complex(Complex::Float(NAN));
  }
 if (m->numberOfColumns() != m->numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  // TODO: return identity matrix if i == 0
  int power = c->approximate(context, angleUnit);
  Expression * result = new Complex(Complex::Float(1.0f));
  for (int k = 0; k < power; k++) {
    Expression * operands[2];
    operands[0] = result;
    operands[1] = m;
    Expression * multiplication = new Multiplication(operands, true);
    Expression * newResult = multiplication->evaluate(context, angleUnit);
    delete result;
    result = newResult;
    delete multiplication;
  }
  return result;
}

Expression * Power::evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const {
  return new Complex(Complex::Float(NAN));
}

Expression * Power::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  return new Complex(Complex::Float(NAN));
}

}

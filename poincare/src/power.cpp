extern "C" {
#include <assert.h>
#include <math.h>
}
#include <poincare/power.h>
#include <poincare/multiplication.h>
#include "layout/baseline_relative_layout.h"

float Power::approximate(Context& context, AngleUnit angleUnit) const {
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

ExpressionLayout * Power::createLayout(DisplayMode displayMode) const {
  Expression * indiceOperand = m_operands[1];
  // Delete eventual parentheses of the indice in the pretty print
  if (m_operands[1]->type() == Type::Parenthesis) {
    indiceOperand = (Expression *)m_operands[1]->operand(0);
  }
  return new BaselineRelativeLayout(m_operands[0]->createLayout(displayMode),indiceOperand->createLayout(displayMode), BaselineRelativeLayout::Type::Superscript);
}

Expression * Power::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  if (d->b() != 0.0f) {
    /* First case c and d is complex */
    if (c->b() != 0.0f || c->a() <= 0) {
      return new Complex(NAN);
    }
    /* Second case only d is complex */
    float radius = powf(c->a(), d->a());
    float theta = d->b()*logf(c->a());
    return new Complex(radius, theta, true);
  }
  /* Third case only c is complex */
  float radius = powf(c->r(), d->a());
  float theta = d->a()*c->th();
  return new Complex(radius, theta, true);
}

Expression * Power::evaluateOnMatrixAndComplex(Matrix * m, Complex * c, Context& context, AngleUnit angleUnit) const {
  if (m_operands[1]->type() != Expression::Type::Integer) {
    return nullptr;
  }
 if (m->numberOfColumns() != m->numberOfRows()) {
    return nullptr;
  }
  // TODO: return identity matrix if i == 0
  int power = c->approximate(context, angleUnit);
  Expression * result = new Complex(1);
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
  return nullptr;
}

Expression * Power::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  return nullptr;
}

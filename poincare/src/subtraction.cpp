extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/subtraction.h>
#include <poincare/opposite.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression * Subtraction::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 2);
  return new Subtraction(newOperands, cloneOperands);
}

float Subtraction::approximate(Context& context, AngleUnit angleUnit) const {
  return m_operands[0]->approximate(context, angleUnit) - m_operands[1]->approximate(context, angleUnit);
}

Expression::Type Subtraction::type() const {
  return Expression::Type::Subtraction;
}

ExpressionLayout * Subtraction::createLayout(DisplayMode displayMode) const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout(displayMode);
  char string[2] = {'-', '\0'};
  children_layouts[1] = new StringLayout(string, 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(displayMode)) : m_operands[1]->createLayout(displayMode);
  return new HorizontalLayout(children_layouts, 3);
}

Expression * Subtraction::evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const {
  return new Complex(c->a() - d->a(), c->b() - d->b());
}

Expression * Subtraction::evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const {
  Expression * operand = evaluateOnMatrixAndComplex(m, c, context, angleUnit);
  Opposite * opposite = new Opposite(operand, false);
  Expression * result = opposite->evaluate(context, angleUnit);
  delete opposite;
  return result;
}

}

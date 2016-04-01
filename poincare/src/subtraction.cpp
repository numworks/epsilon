extern "C" {
#include <stdlib.h>
}
#include <poincare/subtraction.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Expression * Subtraction::clone() {
  return new Subtraction(m_operands, true);
}

float Subtraction::approximate(Context& context) {
  return m_operands[0]->approximate(context) - m_operands[1]->approximate(context);
}

Expression::Type Subtraction::type() {
  return Expression::Type::Subtraction;
}

ExpressionLayout * Subtraction::createLayout() {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout();
  char string[2] = {'-', '\0'};
  children_layouts[1] = new StringLayout(string, 1);
  children_layouts[2] = m_operands[1]->createLayout();
  return new HorizontalLayout(children_layouts, 3);
}

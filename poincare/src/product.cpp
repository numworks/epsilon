extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/product.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"

Expression::Type Product::type() const {
  return Expression::Type::Product;
}

ExpressionLayout * Product::createLayout() const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout();
  children_layouts[1] = new StringLayout("*", 1);
  children_layouts[2] = m_operands[1]->createLayout();
  return new HorizontalLayout(children_layouts, 3);
}

float Product::approximate(Context& context) const {
  return m_operands[0]->approximate(context)*m_operands[1]->approximate(context);;
}

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Product(newOperands, cloneOperands);
}

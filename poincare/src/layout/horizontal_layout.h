#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class HorizontalLayout : public ExpressionLayout {
  public:
    HorizontalLayout(Expression * left_expression, char symbol, Expression * right_expression);
    ~HorizontalLayout();
    virtual void draw();
  private:
    ExpressionLayout * m_children[2];
    char m_symbol;
};

#endif

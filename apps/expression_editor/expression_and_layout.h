#ifndef EXPRESSION_EDITOR_EXPRESSION_AND_LAYOUT_H
#define EXPRESSION_EDITOR_EXPRESSION_AND_LAYOUT_H

#include <poincare.h>

namespace ExpressionEditor {

class ExpressionAndLayout {
public:
  ExpressionAndLayout();
  ~ExpressionAndLayout();
  ExpressionAndLayout(const ExpressionAndLayout& other) = delete;
  ExpressionAndLayout(ExpressionAndLayout&& other) = delete;
  ExpressionAndLayout operator=(const ExpressionAndLayout& other) = delete;
  ExpressionAndLayout& operator=(ExpressionAndLayout&& other) = delete;

  Poincare::Expression * expression() { return m_expression; }
  Poincare::ExpressionLayout * expressionLayout() { return m_expressionLayout; }
private:
  Poincare::Expression * m_expression;
  Poincare::ExpressionLayout * m_expressionLayout;
};

}

#endif

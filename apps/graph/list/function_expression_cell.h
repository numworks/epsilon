#ifndef GRAPH_FUNCTION_EXPRESSION_CELL_H
#define GRAPH_FUNCTION_EXPRESSION_CELL_H

#include <escher.h>

namespace Graph {

class FunctionExpressionCell : public EvenOddCell {
public:
  FunctionExpressionCell();
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
  void setTextColor(KDColor color);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  ExpressionView m_expressionView;
};

}

#endif

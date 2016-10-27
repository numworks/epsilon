#ifndef GRAPH_FUNCTION_EXPRESSION_H
#define GRAPH_FUNCTION_EXPRESSION_H

#include <escher.h>
#include "../function.h"

namespace Graph {

class FunctionCell;

class FunctionExpressionView : public EvenOddCell {
public:
  FunctionExpressionView();
  void setFunction(Function * f);
  Function * function();
  void reloadCell() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  static constexpr KDCoordinate k_verticalFunctionMargin = 50-12;
  static constexpr KDCoordinate k_emptyRowHeight = 50;
  constexpr static KDColor k_separatorColor = KDColor(0xEFF2F4);
  constexpr static KDCoordinate k_separatorThickness = 1;
  Function * m_function;
  ExpressionView m_expressionView;
};

}

#endif

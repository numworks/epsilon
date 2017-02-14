#ifndef GRAPH_FUNCTION_EXPRESSION_CELL_H
#define GRAPH_FUNCTION_EXPRESSION_CELL_H

#include <escher.h>
#include "../cartesian_function.h"

namespace Graph {

class FunctionExpressionCell : public EvenOddCell {
public:
  FunctionExpressionCell();
  virtual void setFunction(CartesianFunction * f);
  CartesianFunction * function();
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  CartesianFunction * m_function;
  ExpressionView m_expressionView;
};

}

#endif

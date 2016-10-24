#ifndef GRAPH_FUNCTION_EXPRESSION_H
#define GRAPH_FUNCTION_EXPRESSION_H

#include <escher.h>
#include "../even_odd_cell.h"
#include "../function.h"

namespace Graph {

class FunctionCell;

class FunctionExpressionView : public EvenOddCell {
public:
  FunctionExpressionView();
  void setFunction(Function * f);
  Function * function();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDColor k_separatorColor = KDColor(0xEFF2F4);
  constexpr static KDCoordinate k_separatorThickness = 1;
  Function * m_function;
};

}

#endif

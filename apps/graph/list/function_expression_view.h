#ifndef GRAPH_FUNCTION_EXPRESSION_H
#define GRAPH_FUNCTION_EXPRESSION_H

#include <escher.h>
#include "function_cell.h"

namespace Graph {

class FunctionCell;

class FunctionExpressionView : public FunctionCell {
public:
  FunctionExpressionView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif

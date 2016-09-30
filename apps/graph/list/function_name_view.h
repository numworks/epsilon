#ifndef GRAPH_FUNCTION_NAME_VIEW_H
#define GRAPH_FUNCTION_NAME_VIEW_H

#include <escher.h>
#include "function_cell.h"


class FunctionNameView : public FunctionCell {
public:
  FunctionNameView();

  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static int k_colorIndicatorThickness = 4;
};

#endif

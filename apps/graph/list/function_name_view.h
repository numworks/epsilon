#ifndef GRAPH_FUNCTION_NAME_VIEW_H
#define GRAPH_FUNCTION_NAME_VIEW_H

#include <escher.h>
#include "function_cell.h"

namespace Graph {

class FunctionNameView : public FunctionCell {
public:
  FunctionNameView();

  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDColor k_separatorColor = KDColor(0xEFF2F4);
};

}

#endif

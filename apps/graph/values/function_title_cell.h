#ifndef GRAPH_FUNCTION_TITLE_CELL_H
#define GRAPH_FUNCTION_TITLE_CELL_H

#include <escher.h>
#include "title_cell.h"

namespace Graph {
class FunctionTitleCell : public TitleCell {
public:
  void setColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
 private:
  constexpr static KDCoordinate k_colorIndicatorThickness = 2;
  KDColor m_functionColor;
};

}

#endif

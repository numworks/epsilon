#ifndef ESCHER_BORDERED_CELL_H
#define ESCHER_BORDERED_CELL_H

#include <escher/highlight_cell.h>
#include <escher/metric.h>

class BorderedCell : public HighlightCell {
public:
  using HighlightCell::HighlightCell;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
};

#endif


#ifndef ESCHER_SPACER_CELL_H
#define ESCHER_SPACER_CELL_H

#include <escher/highlight_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>

namespace Escher {

/* A simple non-selectable cell to add some space between other cells. */

class SpacerCell : public HighlightCell {
public:  
  void setHighlighted(bool highlight) override {}
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(0, k_defaultHeight); }
  void drawRect(KDContext * ctx, KDRect rect) const override {
    /* Drawing rects overlap in lists and spacer only redraws its own part. */
    ctx->fillRect(KDRect(rect.left(), rect.top() + k_overlap, rect.width(), rect.height() - 2*k_overlap), Palette::WallScreen);
  }
private:
  bool protectedIsSelectable() override { return false; }
  /* To keep the same space than ButtonWithSeparator we need +1 since rows of
   * table overlap by one pixel. */
  constexpr static KDCoordinate k_overlap = Escher::Metric::CellSeparatorThickness;;
  constexpr static KDCoordinate k_defaultHeight = Escher::Metric::CommonMenuMargin + k_overlap;
};

}

#endif

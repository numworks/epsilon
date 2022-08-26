#ifndef ESCHER_SPACER_H
#define ESCHER_SPACER_H

#include <escher/highlight_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>

namespace Escher {

class Spacer : public HighlightCell {
public:  
  void setHighlighted(bool highlight) override {}
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(0, k_defaultHeight); }
  void drawRect(KDContext * ctx, KDRect rect) const override {
    ctx->fillRect(KDRect(rect.left(), rect.top() + k_overlap, rect.width(), rect.height() - 2*k_overlap), Palette::WallScreen);
  }
private:
  bool protectedIsSelectable() override { return false; }
  /* To have the same space that when spacer was included in button we need +1
   * since rows of table overlap by one pixel. */
  constexpr static KDCoordinate k_overlap = 1;
  constexpr static KDCoordinate k_defaultHeight = Escher::Metric::CommonMenuMargin + k_overlap;
};

}

#endif

#include "cell_with_separator.h"

namespace Settings {

void CellWithSeparator::setHighlighted(bool highlight) {
  cell()->setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void CellWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, Metric::CellSeparatorThickness, bounds().width(), k_margin), Palette::WallScreen);
}

int CellWithSeparator::numberOfSubviews() const {
  return 1;
}

View * CellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return cell();
}

void CellWithSeparator::layoutSubviews(bool force) {
  cell()->setFrame(KDRect(0, k_margin, bounds().width(), bounds().height()-k_margin), force);
}

}

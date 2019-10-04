#include "cell_with_separator.h"

namespace Settings {

void CellWithSeparator::setHighlighted(bool highlight) {
  cell()->setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void CellWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  //ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyBright);
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, m_separatorBelow ? height - k_margin : Metric::CellSeparatorThickness, bounds().width(), k_margin), Palette::WallScreen);
}

int CellWithSeparator::numberOfSubviews() const {
  return 1;
}

View * CellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return cell();
}

void CellWithSeparator::layoutSubviews(bool force) {
  KDRect frame = KDRect(0, m_separatorBelow ? 0 : k_margin, bounds().width(), bounds().height()-k_margin);
  cell()->setFrame(frame, force);
}

}

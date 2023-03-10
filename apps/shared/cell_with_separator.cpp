#include "cell_with_separator.h"

using namespace Escher;

namespace Shared {

void CellWithSeparator::setHighlighted(bool highlight) {
  cell()->setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void CellWithSeparator::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0,
                       separatorAboveCell()
                           ? k_lineThickness
                           : bounds().height() - k_margin - k_lineThickness,
                       bounds().width(), k_margin),
                Palette::WallScreen);
}

int CellWithSeparator::numberOfSubviews() const { return 1; }

View* CellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return cell();
}

void CellWithSeparator::layoutSubviews(bool force) {
  // With the separator, an additional border is visible.
  setChildFrame(
      cell(),
      KDRect(0, separatorAboveCell() ? k_margin + k_lineThickness : 0,
             bounds().width(), bounds().height() - k_margin - k_lineThickness),
      force);
}

/* MinimalSizeForOptimalDisplay being const, we have no way of ensuring that the
 * width of cell's frame is equal to our frame width. We then cannot call cell's
 * minimalSizeForOptimalDisplay and must handle everything here. */
KDSize CellWithSeparator::minimalSizeForOptimalDisplay() const {
  assert(m_frame.width() > 0);
  return KDSize(m_frame.width(), constCell()->minimalHeightForOptimalDisplay() +
                                     k_margin + Metric::CellSeparatorThickness);
}

}  // namespace Shared

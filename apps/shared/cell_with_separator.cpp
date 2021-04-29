#include "cell_with_separator.h"

using namespace Escher;

namespace Shared {

void CellWithSeparator::setHighlighted(bool highlight) {
  cell()->setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void CellWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(
        0,
        separatorAboveCell() ? k_lineThickness : bounds().height() - k_margin - k_lineThickness,
        bounds().width(),
        k_margin
      ), Palette::WallScreen);
}

int CellWithSeparator::numberOfSubviews() const {
  return 1;
}

View * CellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return cell();
}

void CellWithSeparator::layoutSubviews(bool force) {
  // With the separator, an additional border is visible.
  cell()->setFrame(KDRect(
        0,
        separatorAboveCell() ? k_margin + k_lineThickness : 0,
        bounds().width(),
        bounds().height() - k_margin - k_lineThickness
      ), force);
}

}

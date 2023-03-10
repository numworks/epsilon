#include <escher/cell_with_margins.h>

namespace Escher {

CellWithMargins::CellWithMargins(HighlightCell* innerCell)
    : HighlightCell(), m_innerCell(innerCell) {}

KDSize CellWithMargins::minimalSizeForOptimalDisplay() const {
  KDSize innerSize = m_innerCell->minimalSizeForOptimalDisplay();
  return KDSize(innerSize.width() + 2 * Metric::CommonMargin,
                innerSize.height());
}

void CellWithMargins::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  // Draw horizontal margins
  ctx->fillRect(KDRect(0, 0, Metric::CommonMargin, height),
                Palette::WallScreenDark);
  ctx->fillRect(
      KDRect(width - Metric::CommonMargin, 0, Metric::CommonMargin, height),
      Escher::Palette::WallScreenDark);
}

void CellWithMargins::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_innerCell->setHighlighted(highlight);
}

void CellWithMargins::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  setChildFrame(
      m_innerCell,
      KDRect(Metric::CommonMargin, 0, width - 2 * Metric::CommonMargin, height),
      force);
}

}  // namespace Escher

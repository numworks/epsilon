#include "store_cell.h"
#include "escher/metric.h"

namespace Shared {

void StoreCell::setSeparatorLeft(bool separator) {
  if (m_separatorLeft != separator) {
    m_separatorLeft = separator;
    reloadCell();
  }
}

void StoreCell::drawRect(KDContext * ctx, KDRect rect) const {
  HideableEvenOddEditableTextCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, Metric::TableSeparatorThickness, bounds().height());
  if (m_separatorLeft) {
    ctx->fillRect(separatorRect, HideableEvenOddEditableTextCell::hideColor());
  }
}

void StoreCell::layoutSubviews() {
  KDRect boundsThis = bounds();
  editableTextCell()->setFrame(KDRect(boundsThis.left() + Metric::TableSeparatorThickness, boundsThis.top(), boundsThis.width() - Metric::TableSeparatorThickness - k_rightMargin, boundsThis.height()));
}

}


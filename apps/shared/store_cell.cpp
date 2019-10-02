#include "store_cell.h"

namespace Shared {

void StoreCell::drawRect(KDContext * ctx, KDRect rect) const {
  HideableEvenOddEditableTextCell::drawRect(ctx, rect);
  // Draw the separator
  if (m_separatorLeft) {
    ctx->fillRect(separatorRect(bounds()), HideableEvenOddEditableTextCell::hideColor());
  }
}

void StoreCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  editableTextCell()->setFrame(rectWithoutSeparator(KDRect(boundsThis.left(), boundsThis.top(), boundsThis.width() - k_rightMargin, boundsThis.height())), force);
}

void StoreCell::didSetSeparator() {
  reloadCell();
}

}


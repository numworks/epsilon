#include "store_cell.h"

using namespace Escher;

namespace Shared {

void StoreCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddEditableTextCell::drawRect(ctx, rect);
  // Draw the separator
  if (m_separatorLeft) {
    ctx->fillRect(separatorRect(bounds()), EvenOddCell::k_hideColor);
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

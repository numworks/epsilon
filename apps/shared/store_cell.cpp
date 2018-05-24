#include "store_cell.h"

namespace Shared {

void StoreCell::setSeparatorRight(bool separator) {
  if (separatorRight() != separator) {
    StoreSeparatorCell::setSeparatorRight(separator);
    reloadCell();
  }
}

void StoreCell::drawRect(KDContext * ctx, KDRect rect) const {
  HideableEvenOddEditableTextCell::drawRect(ctx, rect);
  // Draw the separator
  if (separatorRight()) {
    ctx->fillRect(KDRect(bounds().width() - k_separatorThickness, 0, k_separatorThickness, bounds().height()), HideableEvenOddEditableTextCell::hideColor());
  }
}

void StoreCell::layoutSubviews() {
  KDRect boundsThis = bounds();
  editableTextCell()->setFrame(KDRect(boundsThis.topLeft(), boundsThis.width() - k_separatorThickness, boundsThis.height()));
}

}


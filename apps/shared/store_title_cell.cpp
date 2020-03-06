#include "store_title_cell.h"
#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

void StoreTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  BufferFunctionTitleCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect r = separatorRect(bounds(), k_colorIndicatorThickness);
  ctx->fillRect(r, m_separatorLeft ? HideableEvenOddEditableTextCell::hideColor() : backgroundColor());
}

void StoreTitleCell::layoutSubviews(bool force) {
  bufferTextView()->setFrame(rectWithoutSeparator(bufferTextViewFrame()), force);
}

void StoreTitleCell::didSetSeparator() {
  reloadCell();
}

}

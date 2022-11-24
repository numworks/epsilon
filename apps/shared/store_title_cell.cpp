#include "store_title_cell.h"

namespace Shared {

StoreTitleCell::StoreTitleCell() :
  BufferFunctionTitleCell(KDFont::Size::Small),
  Separable()
{}

void StoreTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  BufferFunctionTitleCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect r = separatorRect(bounds(), k_colorIndicatorThickness);
  ctx->fillRect(r, m_separatorLeft ? EvenOddCell::k_hideColor : backgroundColor());
}

void StoreTitleCell::layoutSubviews(bool force) {
  bufferTextView()->setFrame(rectWithoutSeparator(bufferTextViewFrame()), force);
}

void StoreTitleCell::didSetSeparator() {
  reloadCell();
}

}

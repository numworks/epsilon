#include "store_title_cell.h"
#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

void StoreTitleCell::setSeparatorRight(bool separator) {
  if (m_separatorRight != separator) {
    m_separatorRight = separator;
    reloadCell();
  }
}

void StoreTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  BufferFunctionTitleCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(bounds().width() - k_separatorThickness, m_separatorRight ? 0 : k_colorIndicatorThickness, k_separatorThickness, bounds().height());
  if (m_separatorRight) {
    ctx->fillRect(separatorRect, HideableEvenOddEditableTextCell::hideColor());
  } else {
    ctx->fillRect(separatorRect, backgroundColor());
  }
}

void StoreTitleCell::layoutSubviews() {
  KDRect textFrame = bufferTextViewFrame();
  bufferTextView()->setFrame(KDRect(textFrame.topLeft(), textFrame.width() - k_separatorThickness, textFrame.height() ));
}

}

#include "store_title_cell.h"
#include "hideable_even_odd_editable_text_cell.h"
#include "escher/metric.h"

namespace Shared {

void StoreTitleCell::setSeparatorLeft(bool separator) {
  if (m_separatorLeft != separator) {
    m_separatorLeft = separator;
    reloadCell();
  }
}

void StoreTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  BufferFunctionTitleCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, m_separatorLeft ? 0 : k_colorIndicatorThickness, Metric::TableSeparatorThickness, bounds().height() - (m_separatorLeft ? 0 : k_colorIndicatorThickness));
  if (m_separatorLeft) {
    ctx->fillRect(separatorRect, HideableEvenOddEditableTextCell::hideColor());
  } else {
    ctx->fillRect(separatorRect, backgroundColor());
  }
}

void StoreTitleCell::layoutSubviews() {
  KDRect textFrame = bufferTextViewFrame();
  bufferTextView()->setFrame(KDRect(textFrame.left() + Metric::TableSeparatorThickness, textFrame.top(), textFrame.width() - Metric::TableSeparatorThickness, textFrame.height()));
}

}

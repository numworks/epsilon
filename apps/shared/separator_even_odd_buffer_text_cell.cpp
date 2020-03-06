#include "separator_even_odd_buffer_text_cell.h"
#include "hideable_even_odd_editable_text_cell.h"
#include <escher/metric.h>

namespace Shared {

void SeparatorEvenOddBufferTextCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddBufferTextCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, Metric::TableSeparatorThickness, bounds().height());
  ctx->fillRect(separatorRect, Shared::HideableEvenOddEditableTextCell::hideColor());
}

void SeparatorEvenOddBufferTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  KDRect frame = KDRect(
  	boundsThis.left() + Metric::TableSeparatorThickness + k_horizontalMargin,
  	boundsThis.top(),
  	boundsThis.width() - Metric::TableSeparatorThickness - 2*k_horizontalMargin,
  	boundsThis.height());
  m_bufferTextView.setFrame(frame, force);
}

}


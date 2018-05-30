#include "calculation_cell.h"
#include "../shared/hideable_even_odd_editable_text_cell.h"
#include "escher/metric.h"

namespace Statistics {

void CalculationCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddBufferTextCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, Metric::TableSeparatorThickness, bounds().height());
  ctx->fillRect(separatorRect, Shared::HideableEvenOddEditableTextCell::hideColor());
}

void CalculationCell::layoutSubviews() {
  KDRect boundsThis = bounds();
  m_bufferTextView.setFrame(KDRect(boundsThis.left() + Metric::TableSeparatorThickness, boundsThis.top(), boundsThis.width() - Metric::TableSeparatorThickness - k_rightMargin, boundsThis.height()));
}

}


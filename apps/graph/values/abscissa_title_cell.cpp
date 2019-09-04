#include "abscissa_title_cell.h"
#include <apps/shared/hideable_even_odd_editable_text_cell.h>
#include "escher/metric.h"

namespace Graph {

void AbscissaTitleCell::setSeparatorLeft(bool separator) {
  if (m_separatorLeft != separator) {
    m_separatorLeft = separator;
    reloadCell();
  }
}

void AbscissaTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddMessageTextCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, Metric::TableSeparatorThickness, bounds().height());
  ctx->fillRect(separatorRect, m_separatorLeft ? Shared::HideableEvenOddEditableTextCell::hideColor() : backgroundColor());
}

void AbscissaTitleCell::layoutSubviews() {
  KDRect rect = bounds();
  m_messageTextView.setFrame(KDRect(rect.left() + (m_separatorLeft ? Metric::TableSeparatorThickness : 0), rect.top(), rect.width() - (m_separatorLeft ? Metric::TableSeparatorThickness : 0), rect.height()));
}

}

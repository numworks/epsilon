#include "abscissa_title_cell.h"
#include <apps/shared/hideable_even_odd_editable_text_cell.h>

namespace Graph {

void AbscissaTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddMessageTextCell::drawRect(ctx, rect);
  // Draw the separator
  if (m_separatorLeft) {
    KDRect r = separatorRect(bounds());
    ctx->fillRect(r, Shared::HideableEvenOddEditableTextCell::hideColor());
  }
}

void AbscissaTitleCell::layoutSubviews(bool force) {
  m_messageTextView.setFrame(rectWithoutSeparator(bounds()), force);
}

void AbscissaTitleCell::didSetSeparator() {
  reloadCell();
}

}

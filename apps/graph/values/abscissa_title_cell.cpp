#include "abscissa_title_cell.h"

using namespace Escher;

namespace Graph {

void AbscissaTitleCell::drawRect(KDContext* ctx, KDRect rect) const {
  EvenOddMessageTextCell::drawRect(ctx, rect);
  // Draw the separator
  if (m_separatorLeft) {
    KDRect r = separatorRect(bounds());
    ctx->fillRect(r, EvenOddCell::k_hideColor);
  }
}

void AbscissaTitleCell::layoutSubviews(bool force) {
  setChildFrame(&m_messageTextView, rectWithoutSeparator(bounds()), force);
}

void AbscissaTitleCell::didSetSeparator() { reloadCell(); }

}  // namespace Graph

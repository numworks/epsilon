#include "separator_even_odd_buffer_text_cell.h"

#include <escher/metric.h>

using namespace Escher;

namespace Shared {

void SeparatorEvenOddBufferTextCell::drawRect(KDContext* ctx,
                                              KDRect rect) const {
  EvenOddBufferTextCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, k_separatorWidth, bounds().height());
  ctx->fillRect(separatorRect, EvenOddCell::k_hideColor);
}

void SeparatorEvenOddBufferTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  KDRect frame =
      KDRect(boundsThis.left() + k_separatorWidth + k_horizontalMargin,
             boundsThis.top(),
             boundsThis.width() - k_separatorWidth - 2 * k_horizontalMargin,
             boundsThis.height());
  setChildFrame(&m_bufferTextView, frame, force);
}

}  // namespace Shared

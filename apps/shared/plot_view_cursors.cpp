#include "plot_view_cursors.h"

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

KDRect WithCursor::cursorFrame(AbstractPlotView * plotView) {
  assert(plotView && m_cursorView && m_cursor);
  if (!(plotView->hasFocus() && std::isfinite(m_cursor->x()) && std::isfinite(m_cursor->y()))) {
    return KDRectZero;
  }
  KDSize size = m_cursorView->minimalSizeForOptimalDisplay();
  Coordinate2D<float> p = plotView->floatToPixel2D(Coordinate2D<float>(m_cursor->x(), m_cursor->y()));

  /* If the cursor is not visible, put its frame to zero, because it might be
   * very far out of the visible frame and thus later overflow KDCoordinate.
   * The "2" factor is a big safety margin. */
  constexpr int maxCursorPixel = KDCOORDINATE_MAX / 2;
  // Assert we are not removing visible cursors
  static_assert((Ion::Display::Width * 2 < maxCursorPixel) && (Ion::Display::Height * 2 < maxCursorPixel), "maxCursorPixel is should be bigger");
  if (std::abs(p.x1()) > maxCursorPixel || std::abs(p.x2()) > maxCursorPixel) {
    return KDRectZero;
  }

  KDCoordinate px = std::round(p.x1());
  px -= (size.width() - 1) / 2;
  KDCoordinate py = std::round(p.x2());
  py -= (size.height() - 1) / 2;
  if (!(KDCOORDINATE_MIN <= px && px <= KDCOORDINATE_MAX && KDCOORDINATE_MIN <= py && py <= KDCOORDINATE_MAX)) {
    return KDRectZero;
  }
  return KDRect(px, py, size);
}

}
}

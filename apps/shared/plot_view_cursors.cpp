#include "plot_view_cursors.h"

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

KDRect WithCursor::cursorFrame(AbstractPlotView * plotView) {
  assert(plotView && m_cursor);
  if (!m_cursorView || !(plotView->hasFocus() && std::isfinite(m_cursor->x()) && std::isfinite(m_cursor->y()))) {
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
  if (std::fabs(p.x1()) > maxCursorPixel || std::fabs(p.x2()) > maxCursorPixel) {
    return KDRectZero;
  }

  KDCoordinate px = std::round(p.x1());
  px -= (size.width() - 1) / 2;
  KDCoordinate py = std::round(p.x2());
  py -= (size.height() - 1) / 2;
  if (!(KDCOORDINATE_MIN <= px && px <= KDCOORDINATE_MAX && KDCOORDINATE_MIN <= py && py <= KDCOORDINATE_MAX)) {
    return KDRectZero;
  }
  if (size.height() == 0) {
    // The cursor is supposed to take up all the available vertical space.
    KDCoordinate plotHeight = plotView->bounds().height() - (plotView->bannerView() ? plotView->bannerView()->bounds().height() : 0);
    return KDRect(px, 0, size.width(), plotHeight);
  }
  return KDRect(px, py, size);
}

}
}

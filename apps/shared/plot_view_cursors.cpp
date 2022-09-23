#include "plot_view_cursors.h"

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

KDRect WithCursor::cursorFrame(AbstractPlotView * plotView) {
  assert(plotView && m_cursorView && m_cursor);
  KDSize size = m_cursorView->minimalSizeForOptimalDisplay();
  Coordinate2D<float> p = plotView->floatToPixel2D(Coordinate2D<float>(m_cursor->x(), m_cursor->y()));
  float px = p.x1() - (size.width() - 1) / 2;
  float py = p.x2() - (size.height() - 1) / 2;
  if (!(KDCOORDINATE_MIN <= px && px <= KDCOORDINATE_MAX && KDCOORDINATE_MIN <= py && py <= KDCOORDINATE_MAX)) {
    return KDRectZero;
  }
  return KDRect(px, py, size);
}

}
}

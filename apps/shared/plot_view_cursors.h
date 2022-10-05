#ifndef SHARED_PLOT_VIEW_CURSORS_H
#define SHARED_PLOT_VIEW_CURSORS_H

#include "plot_view.h"
#include "curve_view_cursor.h"

namespace Shared {
namespace PlotPolicy {

class NoCursor {
protected:
  CursorView * cursorView(const AbstractPlotView *) const { return nullptr; }
  void privateSetCursorView(AbstractPlotView *, CursorView *) {}
  KDRect cursorFrame(AbstractPlotView *) { return KDRectZero; }
};

class WithCursor {
protected:
  CursorView * cursorView(const AbstractPlotView *) const { return m_cursorView; }
  void privateSetCursorView(AbstractPlotView *, CursorView * cursorView) { m_cursorView = cursorView; }
  KDRect cursorFrame(AbstractPlotView * plotView);

  CursorView * m_cursorView;
  CurveViewCursor * m_cursor;
};

}
}

#endif

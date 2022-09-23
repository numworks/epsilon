#ifndef SHARED_PLOT_VIEW_CURSORS_H
#define SHARED_PLOT_VIEW_CURSORS_H

#include "plot_view.h"

namespace Shared {
namespace PlotPolicy {

class NoCursor {
protected:
  CursorView * cursorView(const AbstractPlotView *) const { return nullptr; }
  KDRect cursorFrame(AbstractPlotView *) { return KDRectZero; }
};

}
}

#endif

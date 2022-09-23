#ifndef SHARED_PLOT_VIEW_AXES_H
#define SHARED_PLOT_VIEW_AXES_H

#include "plot_view.h"

namespace Shared {
namespace PlotPolicy {

class NoAxes {
protected:
  void drawAxes(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {}
};

}
}

#endif

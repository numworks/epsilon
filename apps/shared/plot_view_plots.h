#ifndef SHARED_PLOT_VIEW_PLOTS_H
#define SHARED_PLOT_VIEW_PLOTS_H

#include "plot_view.h"

namespace Shared {
namespace PlotPolicy {

class NoPlot {
protected:
  void drawPlot(const AbstractPlotView *, KDContext *, KDRect) const {};
};

}
}

#endif

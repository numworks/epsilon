#ifndef PYTHON_MATPLOTLIB_PLOT_VIEW_H
#define PYTHON_MATPLOTLIB_PLOT_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include "plot_store.h"

namespace Matplotlib {

class PlotView : public Shared::LabeledCurveView {
public:
  PlotView(PlotStore * s) : Shared::LabeledCurveView(s), m_store(s) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  void traceDot(KDContext * ctx, KDRect r, PlotStore::Dot dot) const;
  void traceSegment(KDContext * ctx, KDRect r, PlotStore::Segment segment) const;
  void traceRect(KDContext * ctx, KDRect r, PlotStore::Rect rect) const;
  void traceLabel(KDContext * ctx, KDRect r, PlotStore::Label label) const;
  PlotStore * m_store;
};

}


#endif

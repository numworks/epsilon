#ifndef PYTHON_MATPLOTLIB_PLOT_VIEW_H
#define PYTHON_MATPLOTLIB_PLOT_VIEW_H

#include <apps/shared/plot_view_policies.h>
#include "plot_store.h"

namespace MicroPython {
  class ExecutionEnvironment;
}

namespace Matplotlib {

class OptionalAxes {
protected:
  void drawAxes(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void reloadAxes(Shared::AbstractPlotView * plotView) { m_axes.reloadAxes(plotView); }
  virtual PlotStore * plotStore() const = 0;

private:
  Shared::PlotPolicy::Axes<Shared::PlotPolicy::WithGrid, Shared::PlotPolicy::NoAxis, Shared::PlotPolicy::NoAxis> m_grid;
  Shared::PlotPolicy::Axes<Shared::PlotPolicy::NoGrid, Shared::PlotPolicy::HorizontalLabeledAxis, Shared::PlotPolicy::VerticalLabeledAxis> m_axes;
};

class PyplotPolicy {
public:
  void setMicroPythonExecutionEnvironment(MicroPython::ExecutionEnvironment * env) { m_micropythonEnvironment = env; }

protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void traceDot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Dot dot) const;
  void traceSegment(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Segment segment) const;
  void traceRect(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Rect rect) const;
  void traceLabel(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Label label) const;

  PlotStore * m_store;
  MicroPython::ExecutionEnvironment * m_micropythonEnvironment;
};

class PyplotView : public Shared::PlotView<OptionalAxes, PyplotPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  PyplotView(PlotStore * s);

private:
  PlotStore * plotStore() const override { return m_store; }
};

}


#endif

#ifndef INFERENCE_STATISTIC_INTERVAL_INTERVAL_CURVE_VIEW_H
#define INFERENCE_STATISTIC_INTERVAL_INTERVAL_CURVE_VIEW_H

#include "inference/models/statistic/interval.h"
#include <apps/shared/plot_view_policies.h>

namespace Inference {

class IntervalAxis : public Shared::PlotPolicy::SimpleAxis {
public:
  constexpr static size_t k_bufferSize = Shared::PlotPolicy::AbstractLabeledAxis::k_labelBufferMaxSize;
  constexpr static size_t k_glyphLength = Shared::PlotPolicy::AbstractLabeledAxis::k_labelBufferMaxGlyphLength;

  void reloadAxis(Shared::AbstractPlotView * plotView, Shared::AbstractPlotView::Axis) override;
  float tickPosition(int i, const Shared::AbstractPlotView * plotView, Shared::AbstractPlotView::Axis) const override;
  void drawLabel(int i, float t, const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, Shared::AbstractPlotView::Axis axis, KDColor color = k_color) const override;

private:
  constexpr static int k_numberOfLabels = 2;

  Interval * interval(const Shared::AbstractPlotView * plotView) const { return static_cast<Interval *>(plotView->range()); }

  float m_ticks[k_numberOfLabels];
  char m_labels[k_numberOfLabels][k_bufferSize];
  Shared::AbstractPlotView::RelativePosition m_positionOfLeftLabel;
};

typedef Shared::PlotPolicy::Axes<Shared::PlotPolicy::NoGrid, IntervalAxis, Shared::PlotPolicy::NoAxis> IntervalAxes;

class IntervalPlotPolicy {
protected:
  constexpr static KDCoordinate k_intervalThickness = 1;
  constexpr static KDCoordinate k_mainIntervalThickness = 2;
  constexpr static KDCoordinate k_intervalBoundHalfHeight = 4;

  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  Interval * m_interval;
  const int * m_selectedIntervalIndex;
};

class IntervalCurveView : public Shared::PlotView<IntervalAxes, IntervalPlotPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  IntervalCurveView(Interval * interval, const int * selectedIndex);

  // AbstractPlotView
  void reload(bool resetInterruption = false, bool force = false) override;

private:
  KDColor backgroundColor() const override { return Escher::Palette::WallScreen; }
};

}

#endif

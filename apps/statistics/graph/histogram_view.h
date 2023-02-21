#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <poincare/print_float.h>

#include "../store.h"

namespace Statistics {

class HistogramPlotPolicy : public Shared::PlotPolicy::WithHistogram {
 protected:
  constexpr static KDColor k_notSelectedHistogramColor =
      Escher::Palette::GrayWhite;
  constexpr static KDColor k_notSelectedHistogramBorderColor =
      Escher::Palette::GrayMiddle;
  constexpr static KDColor k_selectedBarColor = Escher::Palette::YellowDark;

  void drawPlot(const Shared::AbstractPlotView *, KDContext *ctx,
                KDRect rect) const;

  Store *m_store;
  int m_series;
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
};

class HistogramView
    : public Shared::PlotView<Shared::PlotPolicy::LabeledXAxis,
                              HistogramPlotPolicy, Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  HistogramView(Store *store, int series, Shared::CurveViewRange *range);

  // AbstractPlotView
  void reload(bool resetInterruption = false, bool force = false);

  void setHighlight(float start, float end);
  void setDisplayLabels(bool display) { m_xAxis.setHidden(!display); }

 private:
  void reloadSelectedBar();
};

}  // namespace Statistics

#endif

#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <apps/constant.h>
#include <apps/shared/plot_view_policies.h>
#include <escher/highlight_cell.h>
#include <poincare/print_float.h>

#include "../store.h"

namespace Statistics {

class HistogramPlotPolicy : public Shared::PlotPolicy::WithHistogram {
 public:
  void setSeries(int series) {
    assert(0 <= series && series < m_store->numberOfActiveSeries());
    m_series = series;
  }

 protected:
  constexpr static KDColor k_notSelectedHistogramColor =
      Escher::Palette::GrayWhite;
  constexpr static KDColor k_notSelectedHistogramBorderColor =
      Escher::Palette::GrayMiddle;
  constexpr static KDColor k_selectedBarColor = Escher::Palette::YellowDark;

  void drawPlot(const Shared::AbstractPlotView*, KDContext* ctx,
                KDRect rect) const;

  Store* m_store;
  int m_series;
  double m_highlightedBarStart;
  double m_highlightedBarEnd;
};

class HistogramView
    : public Shared::PlotView<Shared::PlotPolicy::LabeledXAxis,
                              HistogramPlotPolicy, Shared::PlotPolicy::NoBanner,
                              Shared::PlotPolicy::NoCursor> {
 public:
  HistogramView(Store* store, Shared::CurveViewRange* range);

  // AbstractPlotView
  void reload(bool resetInterruption = false, bool force = false);

  void setBarHighlight(double start, double end);
  void setDisplayLabels(bool display) { m_xAxis.setHidden(!display); }

 private:
  void reloadSelectedBar();
};

class HistogramCell : public Escher::HighlightCell {
 public:
  explicit HistogramCell(HistogramView&& histogram_view)
      : Escher::HighlightCell(), m_view(std::move(histogram_view)) {}

  // Escher::View
  void drawRect(KDContext* ctx, KDRect rect) const override {
    m_view.drawRect(ctx, rect);
  }

  // Transfer HistogramView public API
  void setSeries(int series) { m_view.setSeries(series); }

  // Set bar highlight (which histogram bar is selected)
  void setBarHighlight(double barHighlightStart, double barHighlightEnd) {
    m_view.setBarHighlight(barHighlightStart, barHighlightEnd);
  }

  // Set global cell highlight, that will automatically be managed by the
  // SelectableList
  void setHighlighted(bool highlight) override {
    Escher::HighlightCell::setHighlighted(highlight);
    m_view.setFocus(highlight);
    m_view.setDisplayLabels(highlight);
  }

  void reload() { layoutSubviews(); }

 private:
  // Escher::View
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_view;
  }
  void layoutSubviews(bool force = false) override {
    setChildFrame(&m_view, KDRect(0, 0, bounds().width(), bounds().height()),
                  force);
    m_view.reload();
  }

  HistogramView m_view;
};

}  // namespace Statistics

#endif

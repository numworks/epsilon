#ifndef STATISTICS_HISTOGRAM_CELL_H
#define STATISTICS_HISTOGRAM_CELL_H

#include <escher/highlight_cell.h>

#include "histogram_view.h"

namespace Statistics {

class HistogramCell : public Escher::HighlightCell {
 public:
  HistogramCell(HistogramView&& histogram_view)
      : Escher::HighlightCell(), m_view(std::move(histogram_view)) {}

  // Escher::View
  void drawRect(KDContext* ctx, KDRect rect) const override {
    m_view.drawRect(ctx, rect);
  }

  // HighlightCell
  void setHighlighted(bool highlight) override;

  // Transfer HistogramView public API
  void setSeries(int series) { m_view.setSeries(series); }

  // Set bar highlight (which histogram bar is selected)
  void setBarHighlight(double barHighlightStart, double barHighlightEnd) {
    m_view.setBarHighlight(barHighlightStart, barHighlightEnd);
  }

  void reload() { layoutSubviews(); }

 private:
  // Escher::View
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_view;
  }
  void layoutSubviews(bool force = false) override;

  HistogramView m_view;
};

}  // namespace Statistics

#endif

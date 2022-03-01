#ifndef STATISTICS_FREQUENCY_CURVE_VIEW_H
#define STATISTICS_FREQUENCY_CURVE_VIEW_H

#include "plot_curve_view.h"
namespace Statistics {

class FrequencyCurveView : public PlotCurveView {
public:
  using PlotCurveView::PlotCurveView;
  // Shared::LabeledCurveView
  // Append '%' to vertical axis labels.
  void appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) override;
private:
  bool connectPoints() const override { return true; }
  double valueAtIndex(int series, int * sortedIndex, int i) const override { return m_store->cumulatedFrequencyAtSortedIndex(series, sortedIndex, i); }
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_CURVE_VIEW_H */

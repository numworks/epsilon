#ifndef STATISTICS_HISTOGRAM_RANGE_H
#define STATISTICS_HISTOGRAM_RANGE_H

#include <apps/shared/memoized_curve_view_range.h>

#include "../store.h"

namespace Statistics {

class HistogramRange : public Shared::MemoizedCurveViewRange {
 public:
  HistogramRange(Store* store) : m_store(store) {}
  void setHistogramXMin(float f, bool updateGridUnit) {
    protectedSetXMin(f, updateGridUnit);
  }
  void setHistogramXMax(float f, bool updateGridUnit) {
    protectedSetXMax(f, updateGridUnit);
  }
  // return true if the window has scrolled
  bool scrollToSelectedBarIndex(int series, int index);

  constexpr static double k_maxNumberOfBars = 10000.0;
  constexpr static float k_displayTopMarginRatio = 0.1f;
  constexpr static float k_displayRightMarginRatio = 0.04f;
  constexpr static int k_bottomMargin = 20;
  constexpr static float k_displayLeftMarginRatio = 0.04f;

 private:
  Store* m_store;
};

}  // namespace Statistics

#endif

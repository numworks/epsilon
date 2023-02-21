#include "histogram_range.h"

namespace Statistics {

bool HistogramRange::scrollToSelectedBarIndex(int series, int index) {
  float startSelectedBar = m_store->startOfBarAtIndex(series, index);
  float windowRange = xMax() - xMin();
  float range =
      windowRange / (1 + k_displayLeftMarginRatio + k_displayRightMarginRatio);
  if (xMin() + k_displayLeftMarginRatio * range > startSelectedBar) {
    // Only update the grid unit when setting xMax
    setHistogramXMin(startSelectedBar - k_displayLeftMarginRatio * range,
                     false);
    setHistogramXMax(xMin() + windowRange, true);
    return true;
  }
  float endSelectedBar = m_store->endOfBarAtIndex(series, index);
  if (endSelectedBar > xMax() - k_displayRightMarginRatio * range) {
    setHistogramXMax(endSelectedBar + k_displayRightMarginRatio * range, false);
    setHistogramXMin(xMax() - windowRange, true);
    return true;
  }
  return false;
}

}  // namespace Statistics

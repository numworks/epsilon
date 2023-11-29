#include "histogram_range.h"

namespace Statistics {

bool HistogramRange::scrollToSelectedBarIndex(int series, int index) {
  float startSelectedBar = m_store->startOfBarAtIndex(series, index);
  float windowRange = xMax() - xMin();
  float range =
      windowRange / (1 + k_displayLeftMarginRatio + k_displayRightMarginRatio);
  if (xMin() + k_displayLeftMarginRatio * range > startSelectedBar) {
    float newMin = startSelectedBar - k_displayLeftMarginRatio * range;
    setHistogramRange(newMin, newMin + windowRange);
    return true;
  }
  float endSelectedBar = m_store->endOfBarAtIndex(series, index);
  if (endSelectedBar > xMax() - k_displayRightMarginRatio * range) {
    float newMax = endSelectedBar + k_displayRightMarginRatio * range;
    setHistogramRange(newMax - windowRange, newMax);
    return true;
  }
  return false;
}

}  // namespace Statistics

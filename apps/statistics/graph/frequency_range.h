#ifndef STATISTICS_FREQUENCY_RANGE_H
#define STATISTICS_FREQUENCY_RANGE_H

#include <apps/shared/memoized_curve_view_range.h>

namespace Statistics {

class FrequencyRange : public Shared::MemoizedCurveViewRange {
public:
  // Calibrate the curveView range to respect margins and display all data
  void calibrate(KDCoordinate width, KDCoordinate height, float seriesXMin, float seriesXMax);
private:
  constexpr static float k_yMax = 100.0f;
  constexpr static float k_yMin = 0.0f;
  constexpr static KDCoordinate k_margin = 20;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_RANGE_H */

#ifndef STATISTICS_NORMAL_PROBABILITY_RANGE_H
#define STATISTICS_NORMAL_PROBABILITY_RANGE_H

#include <apps/shared/memoized_curve_view_range.h>

namespace Statistics {

class NormalProbabilityRange : public Shared::MemoizedCurveViewRange {
public:
  // Calibrate the curveView range to respect margins and display all data
  void calibrate(KDCoordinate width, KDCoordinate height, float seriesXMin, float seriesXMax, float yMin, float yMax);
private:
  constexpr static KDCoordinate k_margin = 20;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_RANGE_H */

#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include "plot_controller.h"

namespace Statistics {

class NormalProbabilityController : public PlotController {
public:
  using PlotController::PlotController;
  // PlotControllerDelegate
  bool handleNullFrequencies() const override { return false; }
  int totalValues(int series) const override;
  double valueAtIndex(int series, int i) const override { return m_store->normalProbabilityValueAtIndex(series, i); }
  double resultAtIndex(int series, int i) const override { return m_store->normalProbabilityResultAtIndex(series, i); }
  void computeYBounds(float * yMin, float *yMax) const override;
  // Horizontal labels will always be in the middle
  KDCoordinate horizontalMargin() const override { return k_mediumMargin; }
  KDCoordinate bottomMargin() const override { return k_smallMargin; }
  KDCoordinate topMargin() const override { return k_smallMargin; }
  bool drawSeriesZScoreLine(int series, float * x, float * y, float * u, float * v) const override;

  TELEMETRY_ID("NormalProbability");
private:
  constexpr static int k_maxTotalValues = Store::k_maxNumberOfPairs;
  static_assert(k_maxTotalValues <= INT_MAX, "maxTotalValues is too large.");

  const char * resultMessageTemplate() const override { return "%s%s%*.*ed"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsNormalProbabilityZScore; }
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H */

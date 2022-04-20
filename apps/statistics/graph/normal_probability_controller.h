#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include "plot_controller.h"
#include <limits.h>

namespace Statistics {

class NormalProbabilityController : public PlotController, public Escher::AlternateEmptyViewDefaultDelegate {
public:
  using PlotController::PlotController;

  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override { return !m_store->hasValidSeries(validSerieMethod()); }
  I18n::Message emptyMessage() override { return I18n::Message::NoDataToPlot; }
  Escher::Responder * defaultController() override { return this; }

  // PlotControllerDelegate
  bool handleNullFrequencies() const override { return false; }
  int totalValues(int series) const override { return m_store->totalNormalProbabilityValues(series); }
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
  // Hide series having invalid total values.
  static bool ValidSerieAndTotal(const Shared::DoublePairStore * store, int series) { return store->seriesIsValid(series) && static_cast<const Store *>(store)->totalNormalProbabilityValues(series) > 0; }

  Shared::DoublePairStore::ValidSeries validSerieMethod() const override { return &NormalProbabilityController::ValidSerieAndTotal; };
  const char * resultMessageTemplate() const override { return "%s%s%*.*ed"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsNormalProbabilityZScore; }
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H */

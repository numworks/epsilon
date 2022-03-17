#ifndef STATISTICS_FREQUENCY_CONTROLLER_H
#define STATISTICS_FREQUENCY_CONTROLLER_H

#include "plot_controller.h"

namespace Statistics {

class FrequencyController : public PlotController {
public:
  using PlotController::PlotController;

  // PlotControllerDelegate
  int totalValues(int series, int * sortedIndex) const override { return m_store->totalCumulatedFrequencyValues(series, sortedIndex); }
  double valueAtIndex(int series, int * sortedIndex, int i) const override { return m_store->cumulatedFrequencyValueAtIndex(series, sortedIndex, i); }
  double resultAtIndex(int series, int * sortedIndex, int i) const override { return m_store->cumulatedFrequencyResultAtIndex(series, sortedIndex, i); }
  void computeYBounds(float * yMin, float *yMax) const override;
  void computeXBounds(float * xMin, float *xMax) const override;
  bool connectPoints() const override { return true; }
  // Append '%' to vertical axis labels.
  void appendLabelSuffix(Shared::CurveView::Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) const override;

  TELEMETRY_ID("Frequency");
private:
  const char * resultMessageTemplate() const override { return "%s : %*.*ed%%"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsFrequencyFcc; }
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_CONTROLLER_H */

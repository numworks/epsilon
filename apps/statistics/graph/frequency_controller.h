#ifndef STATISTICS_FREQUENCY_CONTROLLER_H
#define STATISTICS_FREQUENCY_CONTROLLER_H

#include "plot_controller.h"
#include <apps/shared/round_cursor_view.h>

namespace Statistics {

class FrequencyController : public PlotController {
public:
  FrequencyController(
    Escher::Responder * parentResponder,
    Escher::ButtonRowController * header,
    Escher::TabViewController * tabController,
    Escher::StackViewController * stackViewController,
    Escher::ViewController * typeViewController,
    Store * store
  );

  // PlotController
  int totalValues(int series) const override { return m_store->totalCumulatedFrequencyValues(series); }
  double valueAtIndex(int series, int i) const override { return m_store->cumulatedFrequencyValueAtIndex(series, i); }
  double resultAtIndex(int series, int i) const override { return m_store->cumulatedFrequencyResultAtIndex(series, i); }
  bool connectPoints() const override { return true; }
  // Append '%' to vertical axis labels.
  void appendLabelSuffix(Shared::AbstractPlotView::Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) const override;
  // A 0.5 ratio ensures the vertical labels are 0, 20, 40, 60, 80 and 100%
  float labelStepMultiplicator(Shared::AbstractPlotView::Axis axis) const override { return axis == Shared::AbstractPlotView::Axis::Vertical ? 0.5f : 1.0f; }

  TELEMETRY_ID("Frequency");
private:
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;

  /* Return y value at abscissa x.
   * Return first or last result if x is left or right of bounds */
  double yValueAtAbscissa(int series, double x) const;
  double yValueForComputedXValues(int series, int index, double x, double xIndex, double xNextIndex) const;

  // PlotController
  void moveCursorToSelectedIndex() override;
  bool moveSelectionHorizontally(int deltaIndex) override;
  void computeYBounds(float * yMin, float *yMax) const override;
  bool handleNullFrequencies() const override { return true; }
  int nextSubviewWhenMovingVertically(int direction) const override;
  void updateHorizontalIndexAfterSelectingNewSeries(int previousSelectedSeries) override;

  // Horizontal labels will always be in bottom, vertical labels are wider
  KDCoordinate horizontalMargin() const override { return k_largeMargin; }
  KDCoordinate bottomMargin() const override { return k_mediumMargin; }
  KDCoordinate topMargin() const override { return k_smallMargin; }
  const char * resultMessageTemplate() const override { return "%s%s%*.*ed%%"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsFrequencyFcc; }

  Shared::ToggleableRingRoundCursorView m_cursorView;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_CONTROLLER_H */

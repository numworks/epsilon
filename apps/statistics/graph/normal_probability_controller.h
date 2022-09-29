#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include "apps/shared/round_cursor_view.h"
#include "plot_controller.h"
#include <limits.h>

namespace Statistics {

class NormalProbabilityController : public PlotController, public Escher::AlternateEmptyViewDefaultDelegate {
public:
  NormalProbabilityController(
    Escher::Responder * parentResponder,
    Escher::ButtonRowController * header,
    Escher::Responder * tabController,
    Escher::StackViewController * stackViewController,
    Escher::ViewController * typeViewController,
    Store * store
  );

  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override { return !m_store->hasValidSeries(validSerieMethod()); }
  I18n::Message emptyMessage() override { return I18n::Message::TooManyDataPoints; }
  Escher::Responder * defaultController() override { return this; }

  // PlotController
  void moveCursorToSelectedIndex() override;
  int totalValues(int series) const override { return m_store->totalNormalProbabilityValues(series); }
  double valueAtIndex(int series, int i) const override { return m_store->normalProbabilityValueAtIndex(series, i); }
  double resultAtIndex(int series, int i) const override { return m_store->normalProbabilityResultAtIndex(series, i); }
  bool drawSeriesZScoreLine(int series, float * x, float * y, float * u, float * v) const override;

  TELEMETRY_ID("NormalProbability");
private:
  // PlotController
  // Hide series having invalid total values.
  Shared::DoublePairStore::ValidSeries validSerieMethod() const override { return Store::ValidSeriesAndValidTotalNormalProbabilities; };
  bool moveSelectionHorizontally(int deltaIndex) override;
  void computeYBounds(float * yMin, float *yMax) const override;
  bool handleNullFrequencies() const override { return false; }
  // Horizontal labels will always be in the middle
  KDCoordinate horizontalMargin() const override { return k_mediumMargin; }
  KDCoordinate bottomMargin() const override { return k_smallMargin; }
  KDCoordinate topMargin() const override { return k_smallMargin; }
  const char * resultMessageTemplate() const override { return "%s%s%*.*ed"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsNormalProbabilityZScore; }

  Shared::RingCursorView m_cursorView;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H */

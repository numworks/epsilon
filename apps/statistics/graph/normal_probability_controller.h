#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include <limits.h>

#include "apps/shared/round_cursor_view.h"
#include "plot_controller.h"

namespace Statistics {

class NormalProbabilityController : public PlotController,
                                    public Escher::AlternateEmptyViewDelegate {
 public:
  NormalProbabilityController(Escher::Responder *parentResponder,
                              Escher::ButtonRowController *header,
                              Escher::TabViewController *tabController,
                              Escher::StackViewController *stackViewController,
                              Escher::ViewController *typeViewController,
                              Store *store);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override {
    return !m_store->hasActiveSeries(activeSeriesMethod());
  }
  Escher::Responder *responderWhenEmpty() override { return this; }
  // Only two causes : either too many or non-integer frequencies
  I18n::Message emptyMessage() override {
    return m_store->hasActiveSeries(Store::SumOfOccurrencesUnderMax)
               ? I18n::Message::NonIntegerFrequencies
               : I18n::Message::TooManyDataPoints;
  }

  // PlotController
  void moveCursorToSelectedIndex() override;
  int totalValues(int series) const override {
    return m_store->totalNormalProbabilityValues(series);
  }
  double valueAtIndex(int series, int i) const override {
    return m_store->normalProbabilityValueAtIndex(series, i);
  }
  double resultAtIndex(int series, int i) const override {
    return m_store->normalProbabilityResultAtIndex(series, i);
  }
  bool drawSeriesZScoreLine(int series, float *x, float *y, float *u,
                            float *v) const override;

  TELEMETRY_ID("NormalProbability");

 private:
  // PlotController
  void reloadValueInBanner(Poincare::Preferences::PrintFloatMode displayMode,
                           int precision) override;
  // Hide series having invalid total values.
  Shared::DoublePairStore::ActiveSeriesTest activeSeriesMethod()
      const override {
    return Store::ActiveSeriesAndValidTotalNormalProbabilities;
  };
  bool moveSelectionHorizontally(OMG::HorizontalDirection direction) override;
  void computeYBounds(float *yMin, float *yMax) const override;
  bool handleNullFrequencies() const override { return false; }
  // Horizontal labels will always be in the middle
  KDCoordinate horizontalMargin() const override { return k_mediumMargin; }
  KDCoordinate bottomMargin() const override { return k_smallMargin; }
  KDCoordinate topMargin() const override { return k_smallMargin; }
  const char *resultMessageTemplate() const override { return "%s%s%*.*ed"; }
  I18n::Message resultMessage() const override {
    return I18n::Message::StatisticsNormalProbabilityZScore;
  }
  PlotBannerView *bannerView() override { return &m_simpleBannerView; }
  Shared::RingCursorView m_cursorView;
  SimplePlotBannerView m_simpleBannerView;
};

}  // namespace Statistics

#endif

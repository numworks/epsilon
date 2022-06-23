#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <poincare/print_float.h>
#include "../store.h"
#include <apps/constant.h>
#include <apps/shared/labeled_curve_view.h>

namespace Statistics {

class HistogramController;

class HistogramView : public Shared::HorizontallyLabeledCurveView {
public:
  HistogramView(Store * store, int series, Shared::CurveViewRange * curveViewRange);
  int series() const { return m_series; }
  void reload(bool resetInterrupted = false, bool force = false) override;
  void reloadSelectedBar();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlight(float start, float end);
  void setDisplayLabels(bool display) { m_displayLabels = display; }
private:
  constexpr static KDColor k_notSelectedHistogramColor =  Escher::Palette::GrayWhite;
  constexpr static KDColor k_notSelectedHistogramBorderColor = Escher::Palette::GrayMiddle;
  constexpr static KDColor k_selectedBarColor = Escher::Palette::YellowDark;

  Store * m_store;
  static float EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context);
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
  int m_series;
  bool m_displayLabels;
};

}


#endif

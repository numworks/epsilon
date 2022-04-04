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
  HistogramView(HistogramController * controller, Store * store, int series, Shared::CurveViewRange * curveViewRange, KDColor selectedHistogramColor = Escher::Palette::Select, KDColor notSelectedHistogramColor = Escher::Palette::GrayMiddle, KDColor selectedBarColor = Escher::Palette::YellowDark);
  int series() const { return m_series; }
  void reload(bool resetInterrupted = false, bool force = false) override;
  void reloadSelectedBar();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlight(float start, float end);
  void setDisplayLabels(bool display) { m_displayLabels = display; }
private:
  HistogramController * m_controller;
  Store * m_store;
  static float EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context);
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
  int m_series;
  KDColor m_selectedHistogramColor;
  KDColor m_notSelectedHistogramColor;
  KDColor m_selectedBarColor;
  bool m_displayLabels;
};

}


#endif

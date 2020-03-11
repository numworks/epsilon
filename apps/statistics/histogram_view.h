#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include <poincare/print_float.h>
#include "store.h"
#include "../constant.h"
#include "../shared/labeled_curve_view.h"

namespace Statistics {

class HistogramController;

class HistogramView : public Shared::HorizontallyLabeledCurveView {
public:
  HistogramView(HistogramController * controller, Store * store, int series, Shared::BannerView * bannerView, KDColor selectedHistogramColor = Palette::Select, KDColor notSelectedHistogramColor = Palette::GreyMiddle, KDColor selectedBarColor = Palette::YellowDark);
  int series() const { return m_series; }
  void reload() override;
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

#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "store.h"
#include "../constant.h"
#include "../shared/curve_view.h"

namespace Statistics {

class HistogramController;

class HistogramView : public Shared::CurveView {
public:
  HistogramView(HistogramController * controller, Store * store, int series, Shared::BannerView * bannerView);
  int series() const { return m_series; }
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlight(float start, float end);
private:
  char * label(Axis axis, int index) const override;
  HistogramController * m_controller;
  Store * m_store;
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  static float EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context);
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
  int m_series;
};

}


#endif

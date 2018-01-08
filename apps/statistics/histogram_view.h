#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "store.h"
#include "../constant.h"
#include "../shared/curve_view.h"

namespace Statistics {

class HistogramView : public Shared::CurveView {
public:
  HistogramView(Store * store, Shared::BannerView * bannerView);
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlight(float start, float end);
private:
  char * label(Axis axis, int index) const override;
  Store * m_store;
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  static float EvaluateHistogramAtAbscissa(float abscissa, void * model, void * context);
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
};

}


#endif

#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "store.h"
#include "../constant.h"
#include "../curve_view.h"

namespace Statistics {

class HistogramView : public CurveView {
public:
  HistogramView(Store * store, View * bannerView);
  void reloadSelection() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlight(float start, float end);
private:
  char * label(Axis axis, int index) const override;
  float evaluateModelWithParameter(Model * curve, float t) const override;
  Store * m_store;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  float m_highlightedBarStart;
  float m_highlightedBarEnd;
};

}


#endif

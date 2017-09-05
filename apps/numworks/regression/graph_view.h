#ifndef REGRESSION_GRAPH_VIEW_H
#define REGRESSION_GRAPH_VIEW_H

#include <escher.h>
#include "store.h"
#include "apps/constant.h"
#include "apps/shared/curve_view.h"

namespace Regression {

class GraphView : public Shared::CurveView {
public:
  GraphView(Store * store, Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  float evaluateModelWithParameter(Model * curve, float t) const override;
  Store * m_store;
  char m_xLabels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  char m_yLabels[k_maxNumberOfYLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  /* We memoize the a and b of the regression y = ax+b to avoid recomputing them
   * for each stamp of the curve (since the computation is done with double
   * precision) */
  mutable float m_slope;
  mutable float m_yIntercept;
};

}


#endif

#ifndef REGRESSION_GRAPH_VIEW_H
#define REGRESSION_GRAPH_VIEW_H

#include <escher.h>
#include "store.h"
#include "banner_view.h"
#include "../constant.h"
#include "../cursor_view.h"
#include "../curve_view_with_banner_and_cursor.h"

namespace Regression {

class GraphView : public CurveViewWithBannerAndCursor {
public:
  GraphView(Store * store);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_dotSize = 5;
  char * label(Axis axis, int index) const override;
  BannerView * bannerView() override;
  float evaluateModelWithParameter(Model * curve, float t) const override;
  Store * m_store;
  char m_xLabels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  char m_yLabels[k_maxNumberOfYLabels][Constant::FloatBufferSizeInScientificMode];
  BannerView m_bannerView;
};

}


#endif

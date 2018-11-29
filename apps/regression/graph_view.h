#ifndef REGRESSION_GRAPH_VIEW_H
#define REGRESSION_GRAPH_VIEW_H

#include <escher.h>
#include "store.h"
#include "../constant.h"
#include "../shared/curve_view.h"

namespace Regression {

class GraphView : public Shared::CurveView {
public:
  GraphView(Store * store, Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView, Responder * controller);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  Store * m_store;
  char m_xLabels[k_maxNumberOfXLabels][k_labelBufferSize];
  char m_yLabels[k_maxNumberOfYLabels][k_labelBufferSize];
  Responder * m_controller;
};

}

#endif

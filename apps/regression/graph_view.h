#ifndef REGRESSION_GRAPH_VIEW_H
#define REGRESSION_GRAPH_VIEW_H

#include "store.h"
#include "../constant.h"
#include "../shared/labeled_curve_view.h"

namespace Regression {

class GraphView : public Shared::LabeledCurveView {
public:
  GraphView(Store * store, Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  Store * m_store;
};

}

#endif

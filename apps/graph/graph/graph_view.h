#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "graph_window.h"
#include "../../curve_view_with_banner_and_cursor.h"
#include "../../constant.h"
#include "../function_store.h"
#include "../local_context.h"

namespace Graph {

class GraphView : public CurveViewWithBannerAndCursor {
public:
  GraphView(FunctionStore * functionStore, GraphWindow * graphWindow, BannerView * bannerView, CursorView * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Context * context);
  Context * context() const;
private:
  char * label(Axis axis, int index) const override;
  float evaluateModelWithParameter(Model * expression, float abscissa) const override;
  char m_xLabels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  char m_yLabels[k_maxNumberOfYLabels][Constant::FloatBufferSizeInScientificMode];
  FunctionStore * m_functionStore;
  Context * m_context;
};

}

#endif

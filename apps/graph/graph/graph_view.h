#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphView : public Shared::FunctionGraphView {
public:

  GraphView(CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView);
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void drawTangent(bool tangent) { m_tangent = tangent; }
private:
  CartesianFunctionStore * m_functionStore;
  bool m_tangent;
};

}

#endif

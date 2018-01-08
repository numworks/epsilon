#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphView : public Shared::FunctionGraphView {
public:
  enum class Type {
    Default,
    Tangent,
    Integral
  };
  GraphView(CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView);
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  Type type() const { return m_type; }
  void setType(Type type) { m_type = type; }
private:
  CartesianFunctionStore * m_functionStore;
  Type m_type;
};

}

#endif

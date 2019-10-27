#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"

namespace Graph {

class GraphView : public Shared::FunctionGraphView {
public:
  GraphView(Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void drawTangent(bool tangent) { m_tangent = tangent; }
  /* We override setAreaHighlightColor to make it reload nothing as the
   * highlightColor and the non-highlightColor are identical in the graph view
   * of the application graph. We thereby avoid to uselessly reload some part
   * of the graph where the area under the curve is colored. */
  void setAreaHighlightColor(bool highlightColor) override {};
private:
  bool m_tangent;
};

}

#endif

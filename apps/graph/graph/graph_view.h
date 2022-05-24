#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"

namespace Graph {

class GraphView : public Shared::FunctionGraphView {
public:
  /* The step is a fraction of tmax-tmin. We will evaluate the function at
   * every step and if the consecutive dots are close enough, we won't
   * evaluate any more dot within the step. We pick a very strange fraction
   * denominator to avoid evaluating a periodic function periodically. For
   * example, if tstep was (tmax - tmin)/10, the polar function r(θ) = sin(5θ)
   * defined on 0..2π would be evaluated on r(0) = 0, r(π/5) = 0, r(2*π/5) = 0
   * which would lead to no curve at all. With 10.0938275501223, the
   * problematic functions are the functions whose period is proportionned to
   * 10.0938275501223 which are hopefully rare enough.
   * TODO: The drawCurve algorithm should use the derivative function to know
   * how fast the function moves... */
  static constexpr float k_graphStepDenominator = 10.0938275501223f;

  GraphView(Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, Shared::CursorView * cursorView);
  void reload(bool resetInterrupted = false, bool force = false) override;
  void resetCurvesInterrupted() override { m_functionsInterrupted = 0; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void drawTangent(bool tangent) { m_tangent = tangent; }
  /* We override setAreaHighlightColor to make it reload nothing as the
   * highlightColor and the non-highlightColor are identical in the graph view
   * of the application graph. We thereby avoid to uselessly reload some part
   * of the graph where the area under the curve is colored. */
  void setAreaHighlightColor(bool highlightColor) override {};
private:
  bool allFunctionsInterrupted(int numberOfFunctions) const;
  bool functionWasInterrupted(int index) const;
  void setFunctionInterrupted(int index) const;

  mutable uint32_t m_functionsInterrupted;
  bool m_tangent;
};

}

#endif

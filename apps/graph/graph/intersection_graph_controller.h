#ifndef GRAPH_INTERSECTION_GRAPH_CONTROLLER_H
#define GRAPH_INTERSECTION_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class IntersectionGraphController : public CalculationGraphController {
public:
  IntersectionGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
private:
  void reloadBannerView() override;
  Poincare::Solver<double>::Interest specialInterest() const override { return Poincare::Solver<double>::Interest::Intersection; }
  /* FIXME The default implementation for computeNewPointOfInterest does not
   * update m_intersectedRecord. */

  Ion::Storage::Record m_intersectedRecord;
  // Prevent horizontal panning to preserve search interval
  float cursorRightMarginRatio() override { return 0.0f; }
  float cursorLeftMarginRatio() override { return 0.0f; }
};

}

#endif

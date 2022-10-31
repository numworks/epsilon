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
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) override;

  Ion::Storage::Record m_intersectedRecord;
};

}

#endif

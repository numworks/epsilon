#ifndef GRAPH_ROOT_GRAPH_CONTROLLER_H
#define GRAPH_ROOT_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class RootGraphController : public CalculationGraphController {
public:
  RootGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  TELEMETRY_ID("Root");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

}

#endif

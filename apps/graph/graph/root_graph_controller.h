#ifndef GRAPH_ROOT_GRAPH_CONTROLLER_H
#define GRAPH_ROOT_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class RootGraphController : public CalculationGraphController {
public:
  RootGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  TELEMETRY_ID("Root");
private:
  Poincare::Solver<double>::Interest specialInterest() const override { return Poincare::Solver<double>::Interest::Root; }
};

}

#endif

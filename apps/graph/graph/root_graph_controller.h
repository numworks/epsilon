#ifndef GRAPH_ROOT_GRAPH_CONTROLLER_H
#define GRAPH_ROOT_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class RootGraphController : public CalculationGraphController {
public:
  RootGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  void viewWillAppear() override;
  const char * title() override;
  TELEMETRY_ID("Root");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) override;
  // Prevent horizontal panning to preserve search interval
  float cursorRightMarginRatio() override { return 0.0f; }
  float cursorLeftMarginRatio() override { return 0.0f; }
};

}

#endif

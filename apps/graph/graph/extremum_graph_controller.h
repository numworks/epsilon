#ifndef GRAPH_EXTREMUM_GRAPH_CONTROLLER_H
#define GRAPH_EXTREMUM_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class MinimumGraphController : public CalculationGraphController {
public:
  MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  TELEMETRY_ID("Minimum");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

class MaximumGraphController : public CalculationGraphController {
public:
  MaximumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  TELEMETRY_ID("Maximum");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

}

#endif

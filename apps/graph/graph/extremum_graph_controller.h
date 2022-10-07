#ifndef GRAPH_EXTREMUM_GRAPH_CONTROLLER_H
#define GRAPH_EXTREMUM_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class MinimumGraphController : public CalculationGraphController {
public:
  MinimumGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Minimum");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) override;
  // Prevent horizontal panning to preserve search interval
  float cursorRightMarginRatio() override { return 0.0f; }
  float cursorLeftMarginRatio() override { return 0.0f; }
};

class MaximumGraphController : public CalculationGraphController {
public:
  MaximumGraphController(Escher::Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Maximum");
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) override;
  // Prevent horizontal panning to preserve search interval
  float cursorRightMarginRatio() override { return 0.0f; }
  float cursorLeftMarginRatio() override { return 0.0f; }
};

}

#endif

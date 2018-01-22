#ifndef GRAPH_INTERSECTION_GRAPH_CONTROLLER_H
#define GRAPH_INTERSECTION_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"

namespace Graph {

class IntersectionGraphController : public CalculationGraphController {
public:
  IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, CartesianFunctionStore * functionStore);
  const char * title() override;
private:
  void reloadBannerView() override;
  CartesianFunction::Point computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
  Shared::Function * m_intersectedFunction;
  CartesianFunctionStore * m_functionStore;
};

}

#endif

#ifndef GRAPH_STORAGE_INTERSECTION_GRAPH_CONTROLLER_H
#define GRAPH_STORAGE_INTERSECTION_GRAPH_CONTROLLER_H

#include "storage_calculation_graph_controller.h"

namespace Graph {

class StorageIntersectionGraphController : public StorageCalculationGraphController {
public:
  StorageIntersectionGraphController(Responder * parentResponder, StorageGraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, StorageCartesianFunctionStore * functionStore);
  const char * title() override;
private:
  void reloadBannerView() override;
  Poincare::Expression::Coordinate2D computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
  Shared::StorageCartesianFunction m_intersectedFunction;
  StorageCartesianFunctionStore * m_functionStore;
};

}

#endif

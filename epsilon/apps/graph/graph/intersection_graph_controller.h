#pragma once

#include "calculation_graph_controller.h"

namespace Graph {

class IntersectionGraphController : public CalculationGraphController {
 public:
  IntersectionGraphController(Escher::Responder* parentResponder,
                              GraphView* graphView, BannerView* bannerView,
                              Shared::InteractiveCurveViewRange* curveViewRange,
                              Shared::CurveViewCursor* cursor);
  const char* title() const override;

 private:
  void reloadBannerView() override;
  Poincare::Solver<double>::Interest specialInterest() const override {
    return Poincare::Solver<double>::Interest::Intersection;
  }
  Poincare::Coordinate2D<double> computeNewPointOfInterest(
      double start, double max, bool stretch) override;

  Ion::Storage::Record m_intersectedRecord;
};

}  // namespace Graph

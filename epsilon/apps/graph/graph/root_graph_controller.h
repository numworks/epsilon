#pragma once

#include "calculation_graph_controller.h"

namespace Graph {

class RootGraphController : public CalculationGraphController {
 public:
  RootGraphController(Escher::Responder* parentResponder, GraphView* graphView,
                      BannerView* bannerView,
                      Shared::InteractiveCurveViewRange* curveViewRange,
                      Shared::CurveViewCursor* cursor);
  const char* title() const override;

 private:
  Poincare::Interest specialInterest() const override {
    return Poincare::Interest::Root;
  }
};

}  // namespace Graph

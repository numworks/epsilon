#pragma once

#include "calculation_graph_controller.h"

namespace Graph {

class MinimumGraphController : public CalculationGraphController {
 public:
  MinimumGraphController(Escher::Responder* parentResponder,
                         GraphView* graphView, BannerView* bannerView,
                         Shared::InteractiveCurveViewRange* curveViewRange,
                         Shared::CurveViewCursor* cursor);
  const char* title() const override;

 private:
  Poincare::Interest specialInterest() const override {
    return Poincare::Interest::LocalMinimum;
  }
};

class MaximumGraphController : public CalculationGraphController {
 public:
  MaximumGraphController(Escher::Responder* parentResponder,
                         GraphView* graphView, BannerView* bannerView,
                         Shared::InteractiveCurveViewRange* curveViewRange,
                         Shared::CurveViewCursor* cursor);
  const char* title() const override;

 private:
  Poincare::Interest specialInterest() const override {
    return Poincare::Interest::LocalMaximum;
  }
};

}  // namespace Graph

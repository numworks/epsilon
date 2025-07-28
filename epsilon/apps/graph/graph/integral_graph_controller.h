#ifndef GRAPH_INTEGRAL_GRAPH_CONTROLLER_H
#define GRAPH_INTEGRAL_GRAPH_CONTROLLER_H

#include <apps/shared/sum_graph_controller.h>

#include "graph_view.h"

namespace Graph {

class IntegralGraphController : public Shared::SumGraphController {
 public:
  IntegralGraphController(Escher::Responder* parentResponder,
                          GraphView* graphView,
                          Shared::InteractiveCurveViewRange* graphRange,
                          Shared::CurveViewCursor* cursor);
  const char* title() const override;

 protected:
  double cursorNextStep(double position,
                        OMG::HorizontalDirection direction) override;

 private:
  CodePoint sumSymbol() const override { return UCodePointIntegral; }
  I18n::Message legendMessageAtStep(Step step) override;
  Poincare::Layout createFunctionLayout() override;
};

}  // namespace Graph

#endif

#ifndef GRAPH_INTEGRAL_GRAPH_CONTROLLER_H
#define GRAPH_INTEGRAL_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "../../shared/sum_graph_controller.h"

namespace Graph {

class IntegralGraphController : public Shared::SumGraphController {
public:
  IntegralGraphController(Responder * parentResponder, GraphView * graphView, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
private:
  I18n::Message legendMessageAtStep(Step step) override;
  double cursorNextStep(double position, int direction) override;
  Poincare::ExpressionLayout * createFunctionLayout(const char * functionName) override;
};

}

#endif

#ifndef GRAPH_AREA_BETWEEN_CURVES_GRAPH_CONTROLLER_H
#define GRAPH_AREA_BETWEEN_CURVES_GRAPH_CONTROLLER_H

#include "integral_graph_controller.h"

namespace Graph {

class AreaBetweenCurvesGraphController : public IntegralGraphController {
public:
  AreaBetweenCurvesGraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor) : IntegralGraphController(parentResponder, inputEventHandlerDelegate, graphView, graphRange, cursor) {}
  const char * title() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void setSecondRecord(Ion::Storage::Record record);
private:
  void makeCursorVisible() override;
  double cursorNextStep(double position, OMG::NewHorizontalDirection direction) override;
  Poincare::Layout createFunctionLayout() override;
  Poincare::Expression createSumExpression(double startSum, double endSum, Poincare::Context * context) override;
  Ion::Storage::Record secondSelectedRecord() const { return m_graphView->secondSelectedRecord(); }
};

}

#endif

#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include "../shared/go_to_parameter_controller.h"
#include "store.h"

namespace Regression {

class GraphController;

class GoToParameterController : public Shared::GoToParameterController {
public:
  GoToParameterController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  void setXPrediction(bool xPrediction);
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTwoTitles; }

private:
  double extractParameterAtIndex(int index) override;
  bool confirmParameterAtIndex(int parameterIndex, double f) override;
  Store * m_store;
  bool m_xPrediction;
  GraphController * m_graphController;
};

}

#endif

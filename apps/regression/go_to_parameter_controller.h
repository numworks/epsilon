#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/go_to_parameter_controller.h"
#include "store.h"

namespace Regression {

class GraphController;

class GoToParameterController : public Shared::GoToParameterController {
public:
  GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  void setXPrediction(bool xPrediction);
  const char * title() override;
private:
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Store * m_store;
  bool m_xPrediction;
  GraphController * m_graphController;
};

}

#endif

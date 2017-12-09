#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../shared/go_to_parameter_controller.h"
#include "store.h"

namespace Regression {

class GraphController;

class GoToParameterController final : public Shared::GoToParameterController {
public:
  GoToParameterController(Responder * parentResponder, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController) :
    Shared::GoToParameterController(parentResponder, store, cursor, I18n::Message::X),
    m_store(store),
    m_xPrediction(true),
    m_graphController(graphController) {}
  void setXPrediction(bool xPrediction) {
    m_xPrediction = xPrediction;
  }
  const char * title() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Store * m_store;
  bool m_xPrediction;
  GraphController * m_graphController;
};

}

#endif

#ifndef REGRESSION_GO_TO_PARAMETER_CONTROLLER_H
#define REGRESSION_GO_TO_PARAMETER_CONTROLLER_H

#include <apps/shared/go_to_parameter_controller.h>

#include "../store.h"

namespace Regression {

class GraphController;

class GoToParameterController : public Shared::GoToParameterController {
 public:
  GoToParameterController(Escher::Responder* parentResponder,
                          Shared::InteractiveCurveViewRange* range,
                          Store* store, Shared::CurveViewCursor* cursor,
                          GraphController* graphController);
  void setXPrediction(bool xPrediction);
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }

 private:
  double extractParameterAtIndex(int index) override;
  bool confirmParameterAtIndex(int parameterIndex, double f) override;
  Store* m_store;
  bool m_xPrediction;
  GraphController* m_graphController;
};

}  // namespace Regression

#endif

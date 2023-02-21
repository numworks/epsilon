#ifndef SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"
#include "interactive_curve_view_controller.h"

namespace Shared {

class FunctionGoToParameterController : public GoToParameterController {
 public:
  FunctionGoToParameterController(
      Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      InteractiveCurveViewController* graphController,
      InteractiveCurveViewRange* graphRange, CurveViewCursor* cursor);

  const char* title() override { return I18n::translate(I18n::Message::Goto); }
  void setRecord(Ion::Storage::Record record);

 protected:
  bool confirmParameterAtIndex(int parameterIndex, double f) override;

  Ion::Storage::Record m_record;

 private:
  double extractParameterAtIndex(int index) override {
    assert(index == 0);
    return m_cursor->t();
  }

  InteractiveCurveViewController* m_graphController;
};

}  // namespace Shared

#endif

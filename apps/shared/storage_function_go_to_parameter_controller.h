#ifndef SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"
#include "storage_function.h"

namespace Shared {

class StorageFunctionGoToParameterController : public GoToParameterController {
public:
  StorageFunctionGoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol);
  const char * title() override;
  void setRecord(Ion::Storage::Record record);
protected:
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Ion::Storage::Record m_record;
private:
  double parameterAtIndex(int index) override;
};

}

#endif

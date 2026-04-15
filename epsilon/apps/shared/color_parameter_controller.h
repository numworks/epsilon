#pragma once

#include "abstract_color_parameter_controller.h"
#include "function_store.h"

namespace Shared {

class ColorParameterController : public AbstractColorParameterController {
 public:
  ColorParameterController(Escher::Responder* parentResponder)
      : AbstractColorParameterController(parentResponder),
        m_derivationOrder(0) {}

  // ColorParameterController
  void setRecord(Ion::Storage::Record record, int derivationOrder = 0);

 protected:
  int currentColorIndex() override;
  void setColorIndex(int colorIndex) override;
  void popAfterColorSelection(Escher::StackViewController* stack) override;

 private:
  OMG::ExpiringPointer<Function> function();

  Ion::Storage::Record m_record;
  int m_derivationOrder;
};

}  // namespace Shared

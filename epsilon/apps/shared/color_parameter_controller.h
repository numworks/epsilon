#pragma once

#include "abstract_color_parameter_controller.h"
#include "function_store.h"

namespace Shared {

class ColorParameterController : public AbstractColorParameterController {
 public:
  ColorParameterController(Escher::Responder* parentResponder)
      : AbstractColorParameterController(parentResponder) {}

  // ColorParameterController
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 protected:
  int currentColorIndex() const override;
  void setColorIndex(int colorIndex) override;
  void popAfterColorSelection(Escher::StackViewController* stack) override;

 private:
  OMG::ExpiringPointer<Function> function() const;

  Ion::Storage::Record m_record;
};

}  // namespace Shared

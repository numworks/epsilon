#ifndef SHARED_EXPLICIT_FLOAT_PARAMETER_CONTROLLER
#define SHARED_EXPLICIT_FLOAT_PARAMETER_CONTROLLER

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field.h>

#include "parameter_text_field_delegate.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex).
 *
 * TODO: This class almost a copy paste of FloatParameterController
 * and should probably be factorized/removed. */

class ExplicitFloatParameterController
    : public Escher::ExplicitSelectableListViewController,
      public ParameterTextFieldDelegate {
 public:
  ExplicitFloatParameterController(Escher::Responder* parentResponder);
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

 protected:
  Escher::StackViewController* stackController() {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
  virtual double parameterAtIndex(int index) = 0;
  virtual void fillParameterCellAtRow(int row);
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  virtual bool setParameterAtIndex(int parameterIndex, double f) = 0;
  virtual Escher::TextField* textFieldOfCellAtRow(int row) = 0;
};

}  // namespace Shared

#endif

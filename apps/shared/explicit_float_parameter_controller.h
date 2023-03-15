#ifndef SHARED_EXPLICIT_FLOAT_PARAMETER_CONTROLLER
#define SHARED_EXPLICIT_FLOAT_PARAMETER_CONTROLLER

#include <escher/message_table_cell_with_editable_text.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

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
  ExplicitFloatParameterController(Escher::Responder *parentResponder);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;

  void willDisplayCellForIndex(Escher::HighlightCell *cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  bool textFieldShouldFinishEditing(Escher::AbstractTextField *textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField *textField,
                                 const char *text,
                                 Ion::Events::Event event) override;

 protected:
  Escher::StackViewController *stackController() {
    return static_cast<Escher::StackViewController *>(parentResponder());
  }
  virtual float parameterAtIndex(int index) = 0;

 private:
  virtual bool setParameterAtIndex(int parameterIndex, float f) = 0;
  virtual Escher::TextField *textFieldOfCellAtIndex(Escher::HighlightCell *cell,
                                                    int index) = 0;
};

}  // namespace Shared

#endif

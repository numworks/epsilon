#ifndef SHARED_SIMPLE_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_SIMPLE_FLOAT_PARAMETER_CONTROLLER_H

#include "parameter_text_field_delegate.h"
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

template<typename T>
class SimpleFloatParameterController : public Escher::SelectableListViewController, public ParameterTextFieldDelegate {
public:
  SimpleFloatParameterController(Escher::Responder * parentResponder);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int typeAtIndex(int index) override { return k_parameterCellType; }
  int reusableCellCount(int type) override { return reusableParameterCellCount(type); }
  Escher::HighlightCell * reusableCell(int index, int type) override { return reusableParameterCell(index, type); }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;
protected:
  static constexpr int k_parameterCellType = 0;

  enum class InfinityTolerance {
    None,
    PlusInfinity,
    MinusInfinity
  };
  int activeCell() { return selectedRow(); }
  Escher::StackViewController * stackController() { return static_cast<Escher::StackViewController *>(parentResponder()); }
  virtual T parameterAtIndex(int index) = 0;
private:
  virtual InfinityTolerance infinityAllowanceForRow(int row) const { return InfinityTolerance::None; }
  virtual int reusableParameterCellCount(int type) = 0;
  virtual Escher::HighlightCell * reusableParameterCell(int index, int type) = 0;
  virtual bool setParameterAtIndex(int parameterIndex, T f) = 0;
};

}

#endif

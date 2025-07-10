#ifndef SHARED_PARAMETERS_WITH_VALIDATION_CONTROLLER_H
#define SHARED_PARAMETERS_WITH_VALIDATION_CONTROLLER_H

#include <escher/button_cell.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field.h>

#include "parameter_text_field_delegate.h"

namespace Shared {

// A list of editable parameters with an "ok" button
class ParametersWithValidationController
    : public Escher::ListWithTopAndBottomController,
      public ParameterTextFieldDelegate {
 public:
  ParametersWithValidationController(Escher::Responder* parentResponder,
                                     Escher::View* topView = nullptr,
                                     Escher::View* bottomView = nullptr);

  // ListWithTopAndBottomController
  bool handleEvent(Ion::Events::Event event) override;

  // MemoizedListViewDataSource
  int typeAtRow(int row) const override;
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  KDCoordinate separatorBeforeRow(int row) const override {
    return typeAtRow(row) == k_buttonCellType ? k_defaultRowSeparator : 0;
  }

  // ParameterTextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;

 protected:
  constexpr static int k_parameterCellType = 0;
  constexpr static int k_buttonCellType = 1;

  virtual Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                                    int index) = 0;

  Escher::StackViewController* stackController() {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
  virtual void buttonAction();

  Escher::ButtonCell m_okButton;

 private:
  virtual int reusableParameterCellCount(int type) const = 0;
  virtual Escher::HighlightCell* reusableParameterCell(int index, int type) = 0;
};

}  // namespace Shared

#endif

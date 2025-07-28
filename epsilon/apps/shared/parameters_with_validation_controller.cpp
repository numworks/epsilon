#include "parameters_with_validation_controller.h"

#include <poincare/preferences.h>

#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

ParametersWithValidationController::ParametersWithValidationController(
    Responder* parentResponder, View* topView, View* bottomView)
    : ListWithTopAndBottomController(parentResponder, topView, bottomView),
      m_okButton(&m_selectableListView, I18n::Message::Ok,
                 Invocation::Builder<ParametersWithValidationController>(
                     [](ParametersWithValidationController* parameterController,
                        void* sender) {
                       parameterController->buttonAction();
                       return true;
                     },
                     this),
                 ButtonCell::Style::EmbossedLight) {}

bool ParametersWithValidationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

int ParametersWithValidationController::typeAtRow(int row) const {
  if (row == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_parameterCellType;
}

int ParametersWithValidationController::reusableCellCount(int type) const {
  if (type == k_buttonCellType) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

HighlightCell* ParametersWithValidationController::reusableCell(int index,
                                                                int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

KDCoordinate ParametersWithValidationController::nonMemoizedRowHeight(int row) {
  assert(typeAtRow(row) == k_buttonCellType);
  return m_okButton.minimalSizeForOptimalDisplay().height();
}

bool ParametersWithValidationController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down &&
          innerSelectedRow() < numberOfRows() - 1) ||
         (event == Ion::Events::Up && innerSelectedRow() > 0) ||
         MathTextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

void ParametersWithValidationController::buttonAction() {
  StackViewController* stack =
      ParametersWithValidationController::stackController();
  stack->pop();
}

}  // namespace Shared

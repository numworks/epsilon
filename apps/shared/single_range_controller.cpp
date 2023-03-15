#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleRangeController::SingleRangeController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    Shared::MessagePopUpController *confirmPopUpController)
    : FloatParameterController<float>(parentResponder),
      m_confirmPopUpController(confirmPopUpController) {
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setParentResponder(&m_selectableListView);
    m_boundsCells[i].setDelegates(inputEventHandlerDelegate, this);
  }
  m_autoCell.label()->setMessage(I18n::Message::DefaultSetting);
}

void SingleRangeController::viewWillAppear() {
  extractParameters();
  m_boundsCells[0].label()->setMessage(parameterMessage(0));
  m_boundsCells[1].label()->setMessage(parameterMessage(1));
  FloatParameterController<float>::viewWillAppear();
}

HighlightCell *SingleRangeController::reusableCell(int index, int type) {
  if (type == k_autoCellType) {
    return &m_autoCell;
  }
  if (type == k_parameterCellType) {
    return m_boundsCells + index;
  }
  return FloatParameterController<float>::reusableCell(index, type);
}

KDCoordinate SingleRangeController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  HighlightCell *cell =
      type == k_autoCellType ? static_cast<HighlightCell *>(&m_autoCell)
      : type == k_parameterCellType
          ? static_cast<HighlightCell *>(&m_boundsCells[j - 1])
          : nullptr;
  return cell ? heightForCellAtIndex(cell, j)
              : FloatParameterController<float>::nonMemoizedRowHeight(j);
}

void SingleRangeController::willDisplayCellForIndex(Escher::HighlightCell *cell,
                                                    int index) {
  int type = typeAtIndex(index);
  if (type == k_autoCellType) {
    m_autoCell.accessory()->setState(m_autoParam);
    return;
  }
  FloatParameterController<float>::willDisplayCellForIndex(cell, index);
}

bool SingleRangeController::handleEvent(Ion::Events::Event event) {
  if (typeAtIndex(selectedRow()) == k_autoCellType &&
      m_autoCell.enterOnEvent(event)) {
    // Update auto status
    setAutoStatus(!m_autoParam);
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    if (parametersAreDifferent()) {
      m_confirmPopUpController->presentModally();
    } else {
      pop(false);
    }
    return true;
  }
  return FloatParameterController<float>::handleEvent(event);
}

HighlightCell *SingleRangeController::reusableParameterCell(int index,
                                                            int type) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return &m_boundsCells[index - 1];
}

TextField *SingleRangeController::textFieldOfCellAtIndex(HighlightCell *cell,
                                                         int index) {
  assert(typeAtIndex(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView> *>(cell)
      ->textField();
}

void SingleRangeController::buttonAction() {
  confirmParameters();
  pop(true);
}

bool SingleRangeController::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, const char *text,
    Ion::Events::Event event) {
  setAutoStatus(false);
  return FloatParameterController<float>::textFieldDidFinishEditing(
      textField, text, event);
}

float SingleRangeController::parameterAtIndex(int index) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return (index == 1 ? m_rangeParam.min() : m_rangeParam.max());
}

}  // namespace Shared

#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleRangeController::SingleRangeController(
    Responder *parentResponder,
    Shared::MessagePopUpController *confirmPopUpController)
    : FloatParameterController<float>(parentResponder),
      m_autoParam(false),
      m_confirmPopUpController(confirmPopUpController) {
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setParentResponder(&m_selectableListView);
    m_boundsCells[i].setDelegate(this);
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

KDCoordinate SingleRangeController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  HighlightCell *cell =
      type == k_autoCellType ? static_cast<HighlightCell *>(&m_autoCell)
      : type == k_parameterCellType
          ? static_cast<HighlightCell *>(&m_boundsCells[row - 1])
          : nullptr;
  return cell ? protectedNonMemoizedRowHeight(cell, row)
              : FloatParameterController<float>::nonMemoizedRowHeight(row);
}

void SingleRangeController::fillCellForRow(Escher::HighlightCell *cell,
                                           int row) {
  int type = typeAtRow(row);
  if (type == k_autoCellType) {
    m_autoCell.accessory()->setState(m_autoParam);
    return;
  }
  assert(type == k_buttonCellType || type == k_parameterCellType);
  FloatParameterController<float>::fillCellForRow(cell, row);
}

bool SingleRangeController::handleEvent(Ion::Events::Event event) {
  if (typeAtRow(selectedRow()) == k_autoCellType &&
      m_autoCell.canBeActivatedByEvent(event)) {
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

bool SingleRangeController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 1 || parameterIndex == 2);
  parameterIndex == 1 ? m_rangeParam.setMinKeepingValid(f, limit())
                      : m_rangeParam.setMaxKeepingValid(f, limit());
  return true;
}

void SingleRangeController::setRange(float min, float max) {
  m_rangeParam = Range1D::ValidRangeBetween(min, max, limit());
}

TextField *SingleRangeController::textFieldOfCellAtIndex(HighlightCell *cell,
                                                         int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView> *>(cell)
      ->textField();
}

void SingleRangeController::buttonAction() {
  confirmParameters();
  pop(true);
}

bool SingleRangeController::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, event)) {
    setAutoStatus(false);
    return true;
  }
  return false;
}

float SingleRangeController::parameterAtIndex(int index) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return (index == 1 ? m_rangeParam.min() : m_rangeParam.max());
}

void SingleRangeController::setAutoStatus(bool autoParam) {
  if (m_autoParam == autoParam) {
    return;
  }
  m_autoParam = autoParam;
  if (m_autoParam) {
    setAutoRange();
    m_selectableListView.reloadData();
  } else {
    m_selectableListView.reloadCell(0);
  }
}

}  // namespace Shared

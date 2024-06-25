#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

template <typename T>
SingleRangeController<T>::SingleRangeController(
    Responder* parentResponder,
    Shared::MessagePopUpController* confirmPopUpController)
    : FloatParameterController<T>(parentResponder),
      m_autoParam(false),
      m_confirmPopUpController(confirmPopUpController) {
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setParentResponder(&this->m_selectableListView);
    m_boundsCells[i].setDelegate(this);
  }
  m_autoCell.label()->setMessage(I18n::Message::DefaultSetting);
}

template <typename T>
void SingleRangeController<T>::viewWillAppear() {
  extractParameters();
  m_boundsCells[0].label()->setMessage(parameterMessage(0));
  m_boundsCells[1].label()->setMessage(parameterMessage(1));
  FloatParameterController<T>::viewWillAppear();
}

template <typename T>
HighlightCell* SingleRangeController<T>::reusableCell(int index, int type) {
  if (type == k_autoCellType) {
    return &m_autoCell;
  }
  if (type == this->k_parameterCellType) {
    return m_boundsCells + index;
  }
  return FloatParameterController<T>::reusableCell(index, type);
}

template <typename T>
KDCoordinate SingleRangeController<T>::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  HighlightCell* cell =
      type == k_autoCellType ? static_cast<HighlightCell*>(&m_autoCell)
      : type == this->k_parameterCellType
          ? static_cast<HighlightCell*>(&m_boundsCells[row - 1])
          : nullptr;
  return cell ? this->protectedNonMemoizedRowHeight(cell, row)
              : FloatParameterController<T>::nonMemoizedRowHeight(row);
}

template <typename T>
void SingleRangeController<T>::fillCellForRow(Escher::HighlightCell* cell,
                                              int row) {
  int type = typeAtRow(row);
  if (type == k_autoCellType) {
    m_autoCell.accessory()->setState(m_autoParam);
    return;
  }
  assert(type == this->k_buttonCellType || type == this->k_parameterCellType);
  FloatParameterController<T>::fillCellForRow(cell, row);
}

template <typename T>
bool SingleRangeController<T>::handleEvent(Ion::Events::Event event) {
  if (typeAtRow(this->selectedRow()) == k_autoCellType &&
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
  return FloatParameterController<T>::handleEvent(event);
}

template <typename T>
HighlightCell* SingleRangeController<T>::reusableParameterCell(int index,
                                                               int type) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return &m_boundsCells[index - 1];
}

template <typename T>
bool SingleRangeController<T>::setParameterAtIndex(int parameterIndex, T f) {
  assert(parameterIndex == 1 || parameterIndex == 2);
  parameterIndex == 1 ? m_rangeParam.setMinKeepingValid(f, limit())
                      : m_rangeParam.setMaxKeepingValid(f, limit());
  return true;
}

template <typename T>
void SingleRangeController<T>::setRange(T min, T max) {
  m_rangeParam = Range1D<T>::ValidRangeBetween(min, max, limit());
}

template <typename T>
TextField* SingleRangeController<T>::textFieldOfCellAtIndex(HighlightCell* cell,
                                                            int index) {
  assert(typeAtRow(index) == this->k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView>*>(cell)
      ->textField();
}

template <typename T>
void SingleRangeController<T>::buttonAction() {
  confirmParameters();
  pop(true);
}

template <typename T>
bool SingleRangeController<T>::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  if (FloatParameterController<T>::textFieldDidFinishEditing(textField,
                                                             event)) {
    setAutoStatus(false);
    return true;
  }
  return false;
}

template <typename T>
T SingleRangeController<T>::parameterAtIndex(int index) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return (index == 1 ? m_rangeParam.min() : m_rangeParam.max());
}

template <typename T>
void SingleRangeController<T>::setAutoStatus(bool autoParam) {
  if (m_autoParam == autoParam) {
    return;
  }
  m_autoParam = autoParam;
  if (m_autoParam) {
    setAutoRange();
    this->m_selectableListView.reloadData();
  } else {
    this->m_selectableListView.reloadCell(0);
  }
}

template class SingleRangeController<double>;
template class SingleRangeController<float>;

}  // namespace Shared

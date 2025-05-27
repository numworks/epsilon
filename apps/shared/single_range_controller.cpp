#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

template <typename ParentType>
SingleRangeController<ParentType>::SingleRangeController(
    Responder* parentResponder,
    Shared::MessagePopUpController* confirmPopUpController)
    : ParentType(parentResponder),
      m_autoParam(false),
      m_confirmPopUpController(confirmPopUpController) {
  for (int i = 0; i < k_numberOfBoundsCells; i++) {
    m_boundsCells[i].setParentResponder(&this->m_selectableListView);
    m_boundsCells[i].setDelegate(this);
  }
  m_autoCell.label()->setMessage(I18n::Message::DefaultSetting);
}

template <typename ParentType>
void SingleRangeController<ParentType>::viewWillAppear() {
  extractParameters();
  for (int i = 0; i < k_numberOfBoundsCells; i++) {
    m_boundsCells[i].label()->setMessage(parameterMessage(i));
  }
  ParentType::viewWillAppear();
}

template <typename ParentType>
HighlightCell* SingleRangeController<ParentType>::reusableCell(int index,
                                                               int type) {
  if (type == k_autoCellType) {
    return &m_autoCell;
  }
  return ParentType::reusableCell(index, type);
}

template <typename ParentType>
KDCoordinate SingleRangeController<ParentType>::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  HighlightCell* cell =
      type == k_autoCellType ? static_cast<HighlightCell*>(&m_autoCell)
      : type == this->k_parameterCellType
          ? static_cast<HighlightCell*>(&m_boundsCells[row - 1])
          : nullptr;
  return cell ? this->protectedNonMemoizedRowHeight(cell, row)
              : ParentType::nonMemoizedRowHeight(row);
}

template <typename ParentType>
void SingleRangeController<ParentType>::fillCellForRow(
    Escher::HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type == k_autoCellType) {
    m_autoCell.accessory()->setState(m_autoParam);
    return;
  }
  ParentType::fillCellForRow(cell, row);
}

template <typename ParentType>
bool SingleRangeController<ParentType>::handleEvent(Ion::Events::Event event) {
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
  return ParentType::handleEvent(event);
}

template <typename ParentType>
HighlightCell* SingleRangeController<ParentType>::reusableParameterCell(
    int index, int type) {
  assert(type == this->k_parameterCellType);
  assert(0 <= index && index < k_numberOfBoundsCells);
  return &m_boundsCells[index];
}

template <typename ParentType>
bool SingleRangeController<ParentType>::setParameterAtIndex(
    int parameterIndex, ParameterType value) {
  int i = parameterIndex - 1;
  assert(0 <= i && i < k_numberOfBoundsCells);
  i == 0 ? m_rangeParam.setMinKeepingValid(FloatType(value), limit())
         : m_rangeParam.setMaxKeepingValid(FloatType(value), limit());
  return true;
}

template <typename ParentType>
void SingleRangeController<ParentType>::setRange(FloatType min, FloatType max) {
  m_rangeParam = Range1D<FloatType>::ValidRangeBetween(min, max, limit());
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

template <typename ParentType>
bool SingleRangeController<ParentType>::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  if (ParentType::textFieldDidFinishEditing(textField, event)) {
    setAutoStatus(false);
    return true;
  }
  return false;
}

template <typename ParentType>
typename SingleRangeController<ParentType>::ParameterType
SingleRangeController<ParentType>::parameterAtIndex(int index) {
  int i = index - 1;
  assert(0 <= i && i < k_numberOfBoundsCells);
  return i == 0 ? ParameterType(m_rangeParam.min())
                : ParameterType(m_rangeParam.max());
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

template class SingleRangeController<FloatParameterController<float>>;
template class SingleRangeController<FloatParameterController<double>>;
template class SingleRangeController<ExpressionParameterController>;

}  // namespace Shared

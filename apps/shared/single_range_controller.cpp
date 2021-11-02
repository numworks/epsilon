#include "single_range_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// SingleRangeController

SingleRangeController::SingleRangeController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange, PopUpController * confirmPopUpController) :
  FloatParameterController<float>(parentResponder),
  m_autoCell(I18n::Message::DefaultSetting),
  m_boundsCells{},
  m_tempRange(*interactiveRange),
  m_range(interactiveRange),
  m_confirmPopUpController(confirmPopUpController)
{
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setController(this);
    m_boundsCells[i].setParentResponder(&m_selectableTableView);
    m_boundsCells[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

void SingleRangeController::viewWillAppear() {
  m_tempRange = *m_range;
  FloatParameterController<float>::viewWillAppear();
}

HighlightCell * SingleRangeController::reusableCell(int index, int type) {
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
  HighlightCell * cell = type == k_autoCellType ? static_cast<HighlightCell *>(&m_autoCell) : type == k_parameterCellType ? static_cast<HighlightCell *>(&m_boundsCells[j - 1]) : nullptr;
  return cell ? heightForCellAtIndex(cell, j) : FloatParameterController<float>::nonMemoizedRowHeight(j);
}

void SingleRangeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_autoCellType) {
    SwitchView * switchView = static_cast<SwitchView *>(const_cast<View *>(m_autoCell.accessoryView()));
    switchView->setState(autoStatus());
    return;
  }
  if (type == k_parameterCellType) {
    LockableEditableCell * castedCell = static_cast<LockableEditableCell *>(cell);
    castedCell->setMessage(index == 1 ? I18n::Message::Minimum : I18n::Message::Maximum);
    KDColor color = autoStatus() ? Palette::GrayDark : KDColorBlack;
    castedCell->setTextColor(color);
    castedCell->textField()->setTextColor(color);
  }
  FloatParameterController<float>::willDisplayCellForIndex(cell, index);
}

bool SingleRangeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left || event == Ion::Events::Back) {
    bool sourceAuto = m_editXRange ? m_range->xAuto() : m_range->yAuto();
    if (m_range->rangeChecksum() != m_tempRange.rangeChecksum() || sourceAuto != autoStatus()) {
      Container::activeApp()->displayModalViewController(m_confirmPopUpController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, Metric::PopUpBottomMargin, Metric::PopUpLeftMargin);
    } else {
      stackController()->pop();
    }
    return true;
  }
  if (selectedRow() == 0 && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    if (m_editXRange) {
      m_tempRange.setXAuto(!m_tempRange.xAuto());
    } else {
      m_tempRange.setYAuto(!m_tempRange.yAuto());
    }
    m_tempRange.computeRanges();
    resetMemoization();
    m_selectableTableView.reloadData();
    return true;
  }
  return FloatParameterController<float>::handleEvent(event);
}

float SingleRangeController::parameterAtIndex(int index) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  index--;
  ParameterGetterPointer getters[] = { &InteractiveCurveViewRange::yMin, &InteractiveCurveViewRange::yMax, &InteractiveCurveViewRange::xMin, &InteractiveCurveViewRange::xMax };
  return (m_tempRange.*getters[index + 2 * m_editXRange])();
}

HighlightCell * SingleRangeController::reusableParameterCell(int index, int type) {
  assert(index >= 1 && index < k_numberOfTextCells + 1);
  return &m_boundsCells[index - 1];
}

bool SingleRangeController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 1 && parameterIndex < k_numberOfTextCells + 1);
  parameterIndex--;
  ParameterSetterPointer setters[] = { &InteractiveCurveViewRange::setYMin, &InteractiveCurveViewRange::setYMax, &InteractiveCurveViewRange::setXMin, &InteractiveCurveViewRange::setXMax };
  (m_tempRange.*setters[parameterIndex + 2 * m_editXRange])(f);
  return true;
}

void SingleRangeController::buttonAction() {
  *m_range = m_tempRange;
  FloatParameterController<float>::buttonAction();
}

// SingleRangeController::LockableEditableCell

Responder * SingleRangeController::LockableEditableCell::responder() {
  return m_controller->autoStatus() ? nullptr : this;
}

}

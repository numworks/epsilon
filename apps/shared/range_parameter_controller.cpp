#include "range_parameter_controller.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

RangeParameterController::RangeParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController<float>(parentResponder),
  m_interactiveRange(interactiveRange),
  m_xRangeCells{},
  m_yRangeCells{},
  m_yAutoCell(I18n::Message::YAuto)
{
  for (int i = 0; i < k_numberOfEditableTextCell; i++) {
    m_xRangeCells[i].setParentResponder(&m_selectableTableView);
    m_xRangeCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
  for (int i = 0; i < k_numberOfConvertibleTextCell; i++) {
    m_yRangeCells[i].setParentResponder(&m_selectableTableView);
    m_yRangeCells[i].setInteractiveCurveViewRange(m_interactiveRange);
    m_yRangeCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

const char * RangeParameterController::title() {
  return I18n::translate(I18n::Message::Axis);
}

int RangeParameterController::numberOfRows() const {
  return k_numberOfTextCell+2;
}

int RangeParameterController::typeAtLocation(int i, int j) {
  if (j == numberOfRows()-1) {
    return 0;
  }
  if (j >= 0 && j < 2) {
    return 1;
  }
  if (j == 2) {
    return 2;
  }
  return 3;
}

void RangeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_interactiveRange->yAuto());
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  I18n::Message labels[k_numberOfTextCell+1] = {I18n::Message::XMin, I18n::Message::XMax, I18n::Message::Default, I18n::Message::YMin, I18n::Message::YMax};
  myCell->setMessage(labels[index]);
  KDColor yColor = m_interactiveRange->yAuto() ? Palette::GreyDark : KDColorBlack;
  KDColor colors[k_numberOfTextCell+1] = {KDColorBlack, KDColorBlack, KDColorBlack, yColor, yColor};
  myCell->setTextColor(colors[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool RangeParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, text, event)) {
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (activeCell() == 2 && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    m_interactiveRange->setYAuto(!m_interactiveRange->yAuto());
    m_selectableTableView.reloadData();
    return true;
  }
  return FloatParameterController::handleEvent(event);
}

float RangeParameterController::parameterAtIndex(int parameterIndex) {
  ParameterGetterPointer getters[k_numberOfTextCell] = {&InteractiveCurveViewRange::xMin,
    &InteractiveCurveViewRange::xMax, &InteractiveCurveViewRange::yMin, &InteractiveCurveViewRange::yMax};
  int index = parameterIndex > 2 ? parameterIndex - 1 : parameterIndex;
  return (m_interactiveRange->*getters[index])();
}

bool RangeParameterController::setParameterAtIndex(int parameterIndex, float f) {
  ParameterSetterPointer setters[k_numberOfTextCell] = {&InteractiveCurveViewRange::setXMin,
    &InteractiveCurveViewRange::setXMax, &InteractiveCurveViewRange::setYMin, &InteractiveCurveViewRange::setYMax};
  int index = parameterIndex > 2 ? parameterIndex - 1 : parameterIndex;
  (m_interactiveRange->*setters[index])(f);
  return true;
}

HighlightCell * RangeParameterController::reusableParameterCell(int index, int type) {
  if (type == 2) {
    assert(index == 0);
    return &m_yAutoCell;
  }
  if (type == 1) {
    assert(index >= 0);
    assert(index < k_numberOfEditableTextCell);
    return &m_xRangeCells[index];
  }
  assert(index >= 0);
  assert(index < k_numberOfConvertibleTextCell);
  return &m_yRangeCells[index];
}

int RangeParameterController::reusableParameterCellCount(int type) {
  if (type == 2) {
    return 1;
  }
  if (type == 1) {
    return k_numberOfEditableTextCell;
  }
  return k_numberOfConvertibleTextCell;
}

}

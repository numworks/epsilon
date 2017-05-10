#include "range_parameter_controller.h"
#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

RangeParameterController::RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController(parentResponder),
  m_interactiveRange(interactiveRange),
  m_rangeCells{},
  m_yAutoCell(nullptr)
{
}

const char * RangeParameterController::title() {
  return I18n::translate(I18n::Message::Axis);
}

int RangeParameterController::numberOfRows() {
  return k_numberOfTextCell+2;
}

int RangeParameterController::typeAtLocation(int i, int j) {
  if (j == numberOfRows()-1) {
    return 0;
  }
  if (j == 2) {
    return 2;
  }
  return 1;
}

void RangeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell->accessoryView();
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
    selectableTableView()->reloadData();
    return true;
  }
  return false;
}

void RangeParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow()) {
    return;
  }
  if (previousSelectedCellY < numberOfRows()-1 && previousSelectedCellY >= 0 && previousSelectedCellY !=2) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    if (t->selectedRow() >= 0) {
      app()->setFirstResponder(t);
    }
  }
  if (t->selectedRow() == numberOfRows()-1) {
    Button * myNewCell = (Button *)t->selectedCell();
    app()->setFirstResponder(myNewCell);
    return;
  }
  if (t->selectedRow() >= 0 && t->selectedRow() !=2) {
    MessageTableCellWithEditableText * myNewCell = (MessageTableCellWithEditableText *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    if ((t->selectedRow() == 0 || t->selectedRow() == 1) || !m_interactiveRange->yAuto()) {
      app()->setFirstResponder(myNewCell);
    }
  }
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (activeCell() == 2 && (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    m_interactiveRange->setYAuto(!m_interactiveRange->yAuto());
    selectableTableView()->reloadData();
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
    return m_yAutoCell;
  }
  assert(index >= 0);
  assert(index < k_numberOfTextCell);
  return m_rangeCells[index];
}

int RangeParameterController::reusableParameterCellCount(int type) {
  if (type == 2) {
    return 1;
  }
  return k_numberOfTextCell;
}

View * RangeParameterController::loadView() {
  SelectableTableView * tableView = (SelectableTableView *)FloatParameterController::loadView();
  m_yAutoCell = new MessageTableCellWithSwitch(I18n::Message::YAuto);
  for (int i = 0; i < k_numberOfTextCell; i++) {
    m_rangeCells[i] = new MessageTableCellWithEditableText(tableView, this, m_draftTextBuffer, I18n::Message::Default);
  }
  return tableView;
}

void RangeParameterController::unloadView(View * view) {
  delete m_yAutoCell;
  m_yAutoCell = nullptr;
  for (int i = 0; i < k_numberOfTextCell; i++) {
    delete m_rangeCells[i];
    m_rangeCells[i] = nullptr;
  }
  FloatParameterController::unloadView(view);
}

}


#include "range_parameter_controller.h"
#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

RangeParameterController::RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController(parentResponder, "Valider"),
  m_interactiveRange(interactiveRange),
  m_rangeCells{PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr),
    PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr)},
  m_yAutoCell(PointerTableCellWithSwitch((char*)"Y auto"))
{
}

const char * RangeParameterController::title() const {
  return "Axes";
}

void RangeParameterController::viewWillAppear() {
  for (int i = 0; i < k_numberOfTextCell; i++) {
    int index = i > 2 ? i + 1 : i;
    m_previousParameters[i] = parameterAtIndex(index);
  }
  m_previousSwitchState = m_interactiveRange->yAuto();
  FloatParameterController::viewWillAppear();
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
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_interactiveRange->yAuto());
    return;
  }
  PointerTableCellWithEditableText * myCell = (PointerTableCellWithEditableText *)cell;
  const char * labels[k_numberOfTextCell+1] = {"Xmin", "Xmax", "", "Ymin", "Ymax"};
  myCell->setText(labels[index]);
  KDColor yColor = m_interactiveRange->yAuto() ? Palette::GreyDark : KDColorBlack;
  KDColor colors[k_numberOfTextCell+1] = {KDColorBlack, KDColorBlack, KDColorBlack, yColor, yColor};
  myCell->setTextColor(colors[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool RangeParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  FloatParameterController::textFieldDidFinishEditing(textField, text);
  m_selectableTableView.reloadData();
  return true;
}

void RangeParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX < numberOfRows()-1 && previousSelectedCellY >= 0 && previousSelectedCellY !=2) {
    PointerTableCellWithEditableText * myCell = (PointerTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedRow() == numberOfRows()-1) {
    Button * myNewCell = (Button *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myNewCell);
    return;
  }
  if (t->selectedRow() >= 0 && t->selectedRow() !=2) {
    PointerTableCellWithEditableText * myNewCell = (PointerTableCellWithEditableText *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    if ((t->selectedRow() == 0 || t->selectedRow() == 1) || !m_interactiveRange->yAuto()) {
      app()->setFirstResponder(myNewCell);
    }
  }
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (activeCell() == 2) {
    if (event == Ion::Events::OK) {
      m_interactiveRange->setYAuto(!m_interactiveRange->yAuto());
      m_selectableTableView.reloadData();
      return true;
    }
    return false;
  }
  if (m_interactiveRange->yAuto() && (activeCell() == 3 || activeCell() == 4)) {
    return false;
  }
  if (event == Ion::Events::Back) {
    m_interactiveRange->setYAuto(m_previousSwitchState);
  }
  return FloatParameterController::handleEvent(event);
}

float RangeParameterController::previousParameterAtIndex(int parameterIndex) {
  int index = parameterIndex > 2 ? parameterIndex - 1 : parameterIndex;
  return m_previousParameters[index];
}

float RangeParameterController::parameterAtIndex(int parameterIndex) {
  ParameterGetterPointer getters[k_numberOfTextCell] = {&InteractiveCurveViewRange::xMin,
    &InteractiveCurveViewRange::xMax, &InteractiveCurveViewRange::yMin, &InteractiveCurveViewRange::yMax};
  int index = parameterIndex > 2 ? parameterIndex - 1 : parameterIndex;
  return (m_interactiveRange->*getters[index])();
}

void RangeParameterController::setParameterAtIndex(int parameterIndex, float f) {
  ParameterSetterPointer setters[k_numberOfTextCell] = {&InteractiveCurveViewRange::setXMin,
    &InteractiveCurveViewRange::setXMax, &InteractiveCurveViewRange::setYMin, &InteractiveCurveViewRange::setYMax};
  int index = parameterIndex > 2 ? parameterIndex - 1 : parameterIndex;
  (m_interactiveRange->*setters[index])(f);
}

HighlightCell * RangeParameterController::reusableParameterCell(int index, int type) {
  if (type == 2) {
    return &m_yAutoCell;
  }
  assert(index >= 0);
  assert(index < k_numberOfTextCell);
  return &m_rangeCells[index];
}

int RangeParameterController::reusableParameterCellCount(int type) {
  if (type == 2) {
    return 1;
  }
  return k_numberOfTextCell;
}

}

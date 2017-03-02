#include "range_parameter_controller.h"
#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

RangeParameterController::RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController(parentResponder),
  m_interactiveRange(interactiveRange),
  m_rangeCells{PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Xmin"), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Xmax"),
    PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Ymin"), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Ymax")},
  m_yAutoCell(PointerTableCellWithSwitch((char*)"Y auto"))
{
}

const char * RangeParameterController::title() const {
  return "Axes";
}

void RangeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_interactiveRange->yAuto());
    return;
  }
  if (index == 3 || index == 4) {
    if (m_interactiveRange->yAuto()) {
      m_rangeCells[index-1].setTextColor(Palette::GreyDark);
    } else {
      m_rangeCells[index-1].setTextColor(KDColorBlack);
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool RangeParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  setParameterAtIndex(m_selectableTableView.selectedRow(), floatBody);
  willDisplayCellForIndex(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(),
    m_selectableTableView.selectedRow()), activeCell());
  m_selectableTableView.reloadData();
  return true;
}

void RangeParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == 0 && previousSelectedCellY >= 0 && previousSelectedCellY !=2) {
    PointerTableCellWithEditableText * myCell = (PointerTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedColumn() == 0 && t->selectedRow() >= 0 && t->selectedRow() !=2) {
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
  return false;
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

int RangeParameterController::numberOfRows() {
  return k_numberOfTextCell+1;
};

HighlightCell * RangeParameterController::reusableCell(int index) {
  if (index == 2) {
    return &m_yAutoCell;
  }
  int cellIndex = index > 2 ? index - 1 : index;
  assert(cellIndex >= 0);
  assert(cellIndex < k_numberOfTextCell);
  return &m_rangeCells[cellIndex];
}

int RangeParameterController::reusableCellCount() {
  return k_numberOfTextCell+1;
}

}

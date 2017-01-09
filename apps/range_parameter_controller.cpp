#include "range_parameter_controller.h"
#include "apps_container.h"
#include <assert.h>

RangeParameterController::RangeParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController(parentResponder),
  m_interactiveRange(interactiveRange),
  m_rangeCells{EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Xmin"), EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Xmax"),
    EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Ymin"), EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char *)"Ymax")},
  m_yAutoCell(SwitchMenuListCell((char*)"Y auto"))
{
}

const char * RangeParameterController::title() const {
  return "Axes";
}

void RangeParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_interactiveRange->yAuto());
    return;
  }
  if (index == 3 || index == 4) {
    if (m_interactiveRange->yAuto()) {
      m_rangeCells[index-1].setTextColor(Palette::DesactiveTextColor);
    } else {
      m_rangeCells[index-1].setTextColor(KDColorBlack);
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool RangeParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
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
    EditableTextMenuListCell * myCell = (EditableTextMenuListCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedColumn() == 0 && t->selectedRow() >= 0 && t->selectedRow() !=2) {
    EditableTextMenuListCell * myNewCell = (EditableTextMenuListCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
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

float RangeParameterController::parameterAtIndex(int index) {
  switch (index) {
    case 0:
      return m_interactiveRange->xMin();
    case 1:
      return m_interactiveRange->xMax();
    case 3:
      return m_interactiveRange->yMin();
    case 4:
      return m_interactiveRange->yMax();
    default:
      assert(false);
      return 0.0f;
  }
}

void RangeParameterController::setParameterAtIndex(int parameterIndex, float f) {
  switch(parameterIndex) {
    case 0:
      m_interactiveRange->setXMin(f);
      break;
    case 1:
      m_interactiveRange->setXMax(f);
      break;
    case 3:
      m_interactiveRange->setYMin(f);
      break;
    case 4:
      m_interactiveRange->setYMax(f);
      break;
    default:
      assert(false);
  }
}

int RangeParameterController::numberOfRows() {
  return k_numberOfTextCell+1;
};

TableViewCell * RangeParameterController::reusableCell(int index) {
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

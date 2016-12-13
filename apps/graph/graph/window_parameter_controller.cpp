#include "window_parameter_controller.h"
#include "../app.h"
#include "../../apps_container.h"
#include <assert.h>

namespace Graph {

WindowParameterController::WindowParameterController(Responder * parentResponder, GraphWindow * graphWindow, GraphView * graphView) :
  FloatParameterController(parentResponder),
  m_graphWindow(graphWindow),
  m_yAutoCell(SwitchMenuListCell((char*)"Y auto")),
  m_graphView(graphView)
{
  m_windowCells[0].setText("Xmin");
  m_windowCells[1].setText("Xmax");
  m_windowCells[2].setText("Ymin");
  m_windowCells[3].setText("Ymax");
  for (int k = 0; k < k_numberOfTextCell; k++) {
    m_windowCells[k].setParentResponder(&m_selectableTableView);
    m_windowCells[k].setDelegate(this);
  }
}

ExpressionTextFieldDelegate * WindowParameterController::textFieldDelegate() {
  ExpressionTextFieldDelegate * myApp = (ExpressionTextFieldDelegate *)app();
  return myApp;
}

const char * WindowParameterController::title() const {
  return "Axes";
}

void WindowParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (index == 2) {
    SwitchView * switchView = (SwitchView *)m_yAutoCell.accessoryView();
    switchView->setState(m_graphWindow->yAuto());
    return;
  }
  if (index == 3 || index == 4) {
    if (m_graphWindow->yAuto()) {
      m_windowCells[index-1].setTextColor(Palette::DesactiveTextColor);
    } else {
      m_windowCells[index-1].setTextColor(KDColorBlack);
    }
  }
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool WindowParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->context();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  setParameterAtIndex(m_selectableTableView.selectedRow(), floatBody);
  willDisplayCellForIndex(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(),
    m_selectableTableView.selectedRow()), activeCell());
  m_selectableTableView.reloadData();
  return true;
}

void WindowParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == 0 && previousSelectedCellY >= 0 && previousSelectedCellY !=2) {
    EditableTextMenuListCell * myCell = (EditableTextMenuListCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    app()->setFirstResponder(t);
  }
  if (t->selectedColumn() == 0 && t->selectedRow() >= 0 && t->selectedRow() !=2) {
    EditableTextMenuListCell * myNewCell = (EditableTextMenuListCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    if ((t->selectedRow() == 0 || t->selectedRow() == 1) || !m_graphWindow->yAuto()) {
      app()->setFirstResponder(myNewCell);
    }
  }
}

bool WindowParameterController::handleEvent(Ion::Events::Event event) {
  m_graphView->initCursorPosition();
  if (activeCell() == 2) {
    if (event == Ion::Events::OK) {
      m_graphWindow->setYAuto(!m_graphWindow->yAuto());
      m_selectableTableView.reloadData();
      return true;
    }
    return false;
  }
  if (m_graphWindow->yAuto() && (activeCell() == 3 || activeCell() == 4)) {
    return false;
  }
  return false;
}

float WindowParameterController::parameterAtIndex(int index) {
  switch (index) {
    case 0:
      return m_graphWindow->xMin();
    case 1:
      return m_graphWindow->xMax();
    case 3:
      return m_graphWindow->yMin();
    case 4:
      return m_graphWindow->yMax();
    default:
      assert(false);
      return 0.0f;
  }
}

void WindowParameterController::setParameterAtIndex(int parameterIndex, float f) {
  switch(parameterIndex) {
    case 0:
      m_graphWindow->setXMin(f);
      break;
    case 1:
      m_graphWindow->setXMax(f);
      break;
    case 3:
      m_graphWindow->setYMin(f);
      break;
    case 4:
      m_graphWindow->setYMax(f);
      break;
    default:
      assert(false);
  }
}

int WindowParameterController::numberOfRows() {
  return k_numberOfTextCell+1;
};

TableViewCell * WindowParameterController::reusableCell(int index) {
  if (index == 2) {
    return &m_yAutoCell;
  }
  int cellIndex = index > 2 ? index - 1 : index;
  assert(cellIndex >= 0);
  assert(cellIndex < k_numberOfTextCell);
  return &m_windowCells[cellIndex];
}

int WindowParameterController::reusableCellCount() {
  return k_numberOfTextCell+1;
}

bool WindowParameterController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

}

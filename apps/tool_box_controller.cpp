#include "tool_box_controller.h"
#include <assert.h>

static const char * labels[] = {
  "Abs()",
  "Root()",
  "Log()",
  "Calcul",
  "Nombres complexes",
  "Probabilite",
  "Arithmetiques",
  "Matrices",
  "Listes",
  "Approximation",
  "Trigonometrie hyperbolique"
};

ToolBoxController::ToolBoxController() :
  StackViewController(nullptr, this, true),
  m_selectableTableView(SelectableTableView(this, this))
{
}

const char * ToolBoxController::title() const {
  return "ToolBoxController";
}

bool ToolBoxController::handleEvent(Ion::Events::Event event) {
  return false;
}

void ToolBoxController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int ToolBoxController::numberOfRows() {
  return k_numberOfCommandRows + k_numberOfMenuRows;
}

TableViewCell * ToolBoxController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_numberOfCommandRows);
    return &m_commandCells[index];
  }
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_menuCells[index];
}

int ToolBoxController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return k_numberOfCommandRows;
  }
  return k_maxNumberOfDisplayedRows;
}

void ToolBoxController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuListCell * myCell = (MenuListCell *)cell;
  myCell->setText(labels[index]);
}

KDCoordinate ToolBoxController::rowHeight(int j) {
  if (typeAtLocation(0, j) == 0) {
    return k_commandRowHeight;
  }
  return k_menuRowHeight;
}

KDCoordinate ToolBoxController::cumulatedHeightFromIndex(int j) {
  if (j < k_numberOfCommandRows) {
    return j*k_commandRowHeight;
  }
  return k_commandRowHeight*k_numberOfCommandRows + k_menuRowHeight*(j-k_numberOfCommandRows);
}

int ToolBoxController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY < k_commandRowHeight*k_numberOfCommandRows) {
    return offsetY/k_commandRowHeight;
  }
  return k_numberOfCommandRows + (offsetY - k_commandRowHeight*k_numberOfCommandRows)/k_menuRowHeight;
}

int ToolBoxController::typeAtLocation(int i, int j) {
  if (j < k_numberOfCommandRows) {
    return 0;
  }
  return 1;
}

void ToolBoxController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}

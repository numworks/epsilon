#include "float_parameter_controller.h"
#include "constant.h"
#include "apps_container.h"
#include <assert.h>

FloatParameterController::FloatParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin, this))
{
}

View * FloatParameterController::view() {
  return &m_selectableTableView;
}

void FloatParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int FloatParameterController::activeCell() {
  return m_selectableTableView.selectedRow();
}

void FloatParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  EditableTextMenuListCell * myCell = (EditableTextMenuListCell *) cell;
  char buffer[Constant::FloatBufferSizeInScientificMode];
  Float(parameterAtIndex(index)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  myCell->setAccessoryText(buffer);
}

bool FloatParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  setParameterAtIndex(m_selectableTableView.selectedRow(), floatBody);
  willDisplayCellForIndex(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(),
    m_selectableTableView.selectedRow()), activeCell());
  return true;
}

void FloatParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  EditableTextMenuListCell * myCell = (EditableTextMenuListCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
  myCell->setEditing(false);
  EditableTextMenuListCell * myNewCell = (EditableTextMenuListCell *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
  app()->setFirstResponder(myNewCell);
}

KDCoordinate FloatParameterController::cellHeight() {
  return 35;
}

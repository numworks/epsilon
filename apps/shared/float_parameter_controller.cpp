#include "float_parameter_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "text_field_delegate_app.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

FloatParameterController::FloatParameterController(Responder * parentResponder, I18n::Message okButtonText) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this)),
  m_okButton(ButtonWithSeparator(&m_selectableTableView, okButtonText, Invocation([](void * context, void * sender) {
    FloatParameterController * parameterController = (FloatParameterController *) context;
    parameterController->buttonAction();
  }, this)))
{
}

View * FloatParameterController::view() {
  return &m_selectableTableView;
}

void FloatParameterController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() >= 0) {
    int selectedRow = m_selectableTableView.selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = m_selectableTableView.selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    m_selectableTableView.selectCellAtLocation(selectedColumn, selectedRow);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void FloatParameterController::viewWillAppear() {
  m_selectableTableView.reloadData();
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    int selectedRow = m_selectableTableView.selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = m_selectableTableView.selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    m_selectableTableView.selectCellAtLocation(selectedColumn, selectedRow);
  }
}

void FloatParameterController::willExitResponderChain(Responder * nextFirstResponder) {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

bool FloatParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    for (int i = 0; i < numberOfRows()-1; i++) {
      if (parameterAtIndex(i) != previousParameterAtIndex(i)) {
        setParameterAtIndex(i, previousParameterAtIndex(i));
      }
    }
    stackController()->pop();
    return true;
  }
  return false;
}

int FloatParameterController::typeAtLocation(int i, int j) {
  if (j == numberOfRows()-1) {
    return 0;
  }
  return 1;
}

int FloatParameterController::reusableCellCount(int type) {
  if (type == 0) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

HighlightCell * FloatParameterController::reusableCell(int index, int type) {
  if (type == 0) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

KDCoordinate FloatParameterController::rowHeight(int j) {
  if (j == numberOfRows()-1) {
    return Metric::ParameterCellHeight+k_buttonMargin;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate FloatParameterController::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows()) {
    return j*Metric::ParameterCellHeight+k_buttonMargin;
  }
  return Metric::ParameterCellHeight*j;
}

int FloatParameterController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY > numberOfRows()*Metric::ParameterCellHeight + k_buttonMargin) {
    return numberOfRows();
  }
  return (offsetY - 1) / Metric::ParameterCellHeight;
}

void FloatParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
  if (myCell->isEditing()) {
    return;
  }
  char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex::convertFloatToText(parameterAtIndex(index), buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  myCell->setAccessoryText(buffer);
}

bool FloatParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::parse(text)->approximate(*globalContext);
  if (isnan(floatBody) || isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (!setParameterAtIndex(m_selectableTableView.selectedRow(), floatBody)) {
    return false;
  }
  willDisplayCellForIndex(m_selectableTableView.cellAtLocation(m_selectableTableView.selectedColumn(),
    m_selectableTableView.selectedRow()), activeCell());
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow()+1);
  return true;
}

void FloatParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow()) {
    return;
  }
  if (previousSelectedCellY >= 0 && previousSelectedCellY < numberOfRows()-1) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
  }
  if (t->selectedRow() == numberOfRows()-1) {
    Button * myNewCell = (Button *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myNewCell);
    return;
  }
  if (t->selectedRow() >= 0) {
    MessageTableCellWithEditableText * myNewCell = (MessageTableCellWithEditableText *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myNewCell);
  }
}

TextFieldDelegateApp * FloatParameterController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

int FloatParameterController::activeCell() {
  return m_selectableTableView.selectedRow();
}

StackViewController * FloatParameterController::stackController() {
  return (StackViewController *)parentResponder();
}

void FloatParameterController::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
}

}

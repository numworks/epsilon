#include "float_parameter_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "../shared/poincare_helpers.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

FloatParameterController::FloatParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this),
  m_okButton(&m_selectableTableView, I18n::Message::Ok, Invocation([](void * context, void * sender) {
      FloatParameterController * parameterController = (FloatParameterController *) context;
      parameterController->buttonAction();
      return true;
    }, this))
{
}

void FloatParameterController::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void FloatParameterController::viewWillAppear() {
  if (selectedRow() == -1 || selectedRow() == numberOfRows()-1) {
    selectCellAtLocation(0, 0);
  } else {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  m_selectableTableView.reloadData();
}

void FloatParameterController::willExitResponderChain(Responder * nextFirstResponder) {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

bool FloatParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
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
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  PrintFloat::convertFloatToText<double>(parameterAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
  myCell->setAccessoryText(buffer);
}

bool FloatParameterController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool FloatParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  if (!setParameterAtIndex(selectedRow(), floatBody)) {
    return false;
  }
  m_selectableTableView.reloadCellAtLocation(0, activeCell());
  m_selectableTableView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableTableView.selectCellAtLocation(selectedColumn(), selectedRow()+1);
  } else {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

TextFieldDelegateApp * FloatParameterController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

int FloatParameterController::activeCell() {
  return selectedRow();
}

StackViewController * FloatParameterController::stackController() {
  return (StackViewController *)parentResponder();
}

void FloatParameterController::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
}

}

#include "float_parameter_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

FloatParameterController::FloatParameterController(Responder * parentResponder) :
  DynamicViewController(parentResponder),
  m_okButton(nullptr)
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
  app()->setFirstResponder(selectableTableView());
}

void FloatParameterController::viewWillAppear() {
  DynamicViewController::viewWillAppear();
  if (selectedRow() == -1 || selectedRow() == numberOfRows()-1) {
    selectCellAtLocation(0, 0);
  } else {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  selectableTableView()->reloadData();
}

void FloatParameterController::willExitResponderChain(Responder * nextFirstResponder) {
  if (parentResponder() == nullptr) {
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0,0);
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
    return m_okButton;
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
  PrintFloat::convertFloatToText<double>(parameterAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
  myCell->setAccessoryText(buffer);
}

bool FloatParameterController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool FloatParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  double floatBody = Expression::approximateToScalar<double>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (!setParameterAtIndex(selectedRow(), floatBody)) {
    return false;
  }
  selectableTableView()->reloadCellAtLocation(0, activeCell());
  selectableTableView()->reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(selectedColumn(), selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
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

SelectableTableView * FloatParameterController::selectableTableView() {
  return (SelectableTableView *)view();
}

void FloatParameterController::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
}

I18n::Message FloatParameterController::okButtonText() {
  return I18n::Message::Ok;
}

View * FloatParameterController::loadView() {
  SelectableTableView * tableView = new SelectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, this);
  m_okButton = new ButtonWithSeparator(tableView, okButtonText(), Invocation([](void * context, void * sender) {
    FloatParameterController * parameterController = (FloatParameterController *) context;
    parameterController->buttonAction();
  }, this));
  return tableView;
}

void FloatParameterController::unloadView(View * view) {
  delete m_okButton;
  m_okButton = nullptr;
  delete view;
}

}

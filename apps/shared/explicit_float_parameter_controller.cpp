#include "explicit_float_parameter_controller.h"

#include <assert.h>
#include <poincare/preferences.h>

#include <cmath>

#include "poincare_helpers.h"
#include "text_field_delegate_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

ExplicitFloatParameterController::ExplicitFloatParameterController(
    Responder *parentResponder)
    : ExplicitSelectableListViewController(parentResponder) {}

void ExplicitFloatParameterController::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = std::min(selectedRow(), numberOfRows() - 1);
    selectCell(selRow);
  }
  ExplicitSelectableListViewController::didBecomeFirstResponder();
}

void ExplicitFloatParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  int selRow = selectedRow();
  if (selRow == -1) {
    selectCell(0);
  } else {
    selRow = std::min(selectedRow(), numberOfRows() - 1);
    selectCell(selRow);
  }
  resetMemoization();
  m_selectableListView.reloadData();
}

void ExplicitFloatParameterController::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableListView.deselectTable();
    m_selectableListView.scrollToCell(0);
  }
}

bool ExplicitFloatParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

void ExplicitFloatParameterController::willDisplayCellForIndex(
    HighlightCell *cell, int index) {
  if (textFieldOfCellAtIndex(cell, index)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      parameterAtIndex(index), buffer, bufferSize, precision,
      Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, index)->setText(buffer);
}

KDCoordinate ExplicitFloatParameterController::nonMemoizedRowHeight(int j) {
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

bool ExplicitFloatParameterController::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows() - 1) ||
         (event == Ion::Events::Up && selectedRow() > 0) ||
         TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ExplicitFloatParameterController::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  float floatBody =
      textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  int row = selectedRow();
  if (textFieldDelegateApp()->hasUndefinedValue(floatBody)) {
    return false;
  }
  if (!setParameterAtIndex(row, floatBody)) {
    return false;
  }
  resetMemoization();
  m_selectableListView.reloadCell(selectedRow());
  m_selectableListView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableListView.selectCell(selectedRow() + 1);
  } else {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

}  // namespace Shared

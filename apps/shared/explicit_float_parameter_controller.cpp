#include "explicit_float_parameter_controller.h"

#include <assert.h>
#include <poincare/preferences.h>

#include <cmath>

#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

ExplicitFloatParameterController::ExplicitFloatParameterController(
    Responder* parentResponder)
    : ExplicitSelectableListViewController(parentResponder) {}

void ExplicitFloatParameterController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() >= 0) {
      int selRow = std::min(selectedRow(), numberOfRows() - 1);
      selectRow(selRow);
    }
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  } else {
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  }
}

void ExplicitFloatParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  int nRows = numberOfRows();
  for (int row = 0; row < nRows; row++) {
    if (cell(row)->isVisible()) {
      fillParameterCellAtRow(row);
    }
  }
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

void ExplicitFloatParameterController::fillParameterCellAtRow(int row) {
  if (textFieldOfCellAtRow(row)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      parameterAtIndex(row), buffer, bufferSize, precision,
      Preferences::SharedPreferences()->displayMode());
  textFieldOfCellAtRow(row)->setText(buffer);
}

bool ExplicitFloatParameterController::textFieldShouldFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows() - 1) ||
         (event == Ion::Events::Up && selectedRow() > 0) ||
         MathTextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ExplicitFloatParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  int row = selectedRow();
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  if (!setParameterAtIndex(row, floatBody)) {
    return false;
  }
  m_selectableListView.reloadSelectedCell();
  m_selectableListView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableListView.selectCell(selectedRow() + 1);
  } else {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

}  // namespace Shared

#include "display_mode_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/print_int.h>

#include <cmath>

#include "../app.h"

using namespace Shared;
using namespace Escher;

namespace Settings {

DisplayModeController::DisplayModeController(Responder *parentResponder)
    : PreferencesController(parentResponder),
      m_editableCell(&m_selectableListView, this) {
  m_editableCell.label()->setMessage(I18n::Message::SignificantFigures);
}

KDCoordinate DisplayModeController::nonMemoizedRowHeight(int row) {
  if (row == numberOfRows() - 1) {
    return protectedNonMemoizedRowHeight(&m_editableCell, row);
  }
  return PreferencesController::nonMemoizedRowHeight(row);
}

HighlightCell *DisplayModeController::reusableCell(int index, int type) {
  if (type == k_significantDigitsType) {
    assert(index == 0);
    return &m_editableCell;
  }
  return PreferencesController::reusableCell(index, type);
}

int DisplayModeController::reusableCellCount(int type) const {
  if (type == k_resultFormatType) {
    return PreferencesController::k_totalNumberOfCell;
  }
  assert(type == k_significantDigitsType);
  return 1;
}

void DisplayModeController::fillCellForRow(HighlightCell *cell, int row) {
  /* Number of significants figure row */
  if (typeAtRow(row) == k_significantDigitsType) {
    assert(cell == &m_editableCell);
    GenericSubController::fillCellForRow(cell, row);
    constexpr int bufferSize = 3;
    char buffer[bufferSize];
    int length = Poincare::PrintInt::Left(
        Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits(),
        buffer, bufferSize);
    buffer[length] = 0;
    m_editableCell.textField()->setText(buffer);
    return;
  }
  PreferencesController::fillCellForRow(cell, row);
}

bool DisplayModeController::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return (event == Ion::Events::Up && selectedRow() > 0) ||
         MathTextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool DisplayModeController::textFieldDidFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  if (floatBody < 1.0) {
    floatBody = 1.0;
  }
  if (Poincare::Preferences::SharedPreferences()->displayMode() ==
          Poincare::Preferences::PrintFloatMode::Engineering &&
      floatBody < 3.0) {
    floatBody = 3.0;
  }
  if (floatBody > Poincare::PrintFloat::k_maxNumberOfSignificantDigits) {
    floatBody = Poincare::PrintFloat::k_maxNumberOfSignificantDigits;
  }
  Poincare::Preferences::SharedPreferences()->setNumberOfSignificantDigits(
      (char)std::round(floatBody));
  m_selectableListView.reloadSelectedCell();
  if (event == Ion::Events::Up || event == Ion::Events::OK) {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

}  // namespace Settings

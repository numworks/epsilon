#include "display_mode_controller.h"

#include <assert.h>
#include <poincare/integer.h>

#include <cmath>

#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

DisplayModeController::DisplayModeController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate)
    : PreferencesController(parentResponder),
      m_editableCell(&m_selectableTableView, inputEventHandlerDelegate, this) {
  m_editableCell.messageTableCellWithEditableText()->setMessage(
      I18n::Message::SignificantFigures);
}

KDCoordinate DisplayModeController::nonMemoizedRowHeight(int j) {
  if (j == numberOfRows() - 1) {
    // Do not call heightForCellAtIndex as it will reset edited text.
    return m_editableCell.minimalSizeForOptimalDisplay().height();
  }
  return PreferencesController::nonMemoizedRowHeight(j);
}

HighlightCell *DisplayModeController::reusableCell(int index, int type) {
  if (type == k_significantDigitsType) {
    assert(index == 0);
    return &m_editableCell;
  }
  return PreferencesController::reusableCell(index, type);
}

int DisplayModeController::reusableCellCount(int type) {
  if (type == k_resultFormatType) {
    return PreferencesController::k_totalNumberOfCell;
  }
  assert(type == k_significantDigitsType);
  return 1;
}

void DisplayModeController::willDisplayCellForIndex(HighlightCell *cell,
                                                    int index) {
  /* Number of significants figure row */
  if (index == numberOfRows() - 1) {
    MessageTableCellWithEditableTextWithSeparator *myCell =
        static_cast<MessageTableCellWithEditableTextWithSeparator *>(cell);
    GenericSubController::willDisplayCellForIndex(
        myCell->messageTableCellWithEditableText(), index);
    constexpr int bufferSize = 3;
    char buffer[bufferSize];
    Integer(Preferences::sharedPreferences->numberOfSignificantDigits())
        .serialize(buffer, bufferSize);
    myCell->messageTableCellWithEditableText()->setAccessoryText(buffer);
    return;
  }
  PreferencesController::willDisplayCellForIndex(cell, index);
}

bool DisplayModeController::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return (event == Ion::Events::Up && selectedRow() > 0) ||
         TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool DisplayModeController::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  double floatBody =
      textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (textFieldDelegateApp()->hasUndefinedValue(floatBody)) {
    return false;
  }
  if (floatBody < 1.0) {
    floatBody = 1.0;
  }
  if (Preferences::sharedPreferences->displayMode() ==
          Preferences::PrintFloatMode::Engineering &&
      floatBody < 3.0) {
    floatBody = 3.0;
  }
  if (floatBody > PrintFloat::k_numberOfStoredSignificantDigits) {
    floatBody = PrintFloat::k_numberOfStoredSignificantDigits;
  }
  Preferences::sharedPreferences->setNumberOfSignificantDigits(
      (char)std::round(floatBody));
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  if (event == Ion::Events::Up || event == Ion::Events::OK) {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

}  // namespace Settings

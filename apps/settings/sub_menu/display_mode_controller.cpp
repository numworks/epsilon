#include "display_mode_controller.h"
#include "../../shared/poincare_helpers.h"
#include <assert.h>
#include <cmath>
#include "../app.h"

#include <poincare/integer.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

DisplayModeController::DisplayModeController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  PreferencesController(parentResponder),
  m_editableCell(&m_selectableTableView, inputEventHandlerDelegate, this)
{
  m_editableCell.messageTableCellWithEditableText()->setMessage(I18n::Message::SignificantFigures);
  m_editableCell.messageTableCellWithEditableText()->setMessageFont(KDFont::LargeFont);
}

KDCoordinate DisplayModeController::rowHeight(int j) {
  if (j == numberOfRows()-1) {
    return Metric::ParameterCellHeight+MessageTableCellWithEditableTextWithSeparator::k_margin;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate DisplayModeController::cumulatedHeightFromIndex(int j) {
  return TableViewDataSource::cumulatedHeightFromIndex(j);
}

int DisplayModeController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return TableViewDataSource::indexFromCumulatedHeight(offsetY);
}

HighlightCell * DisplayModeController::reusableCell(int index, int type) {
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

int DisplayModeController::typeAtLocation(int i, int j) {
  return (j == numberOfRows() - 1 ? k_significantDigitsType : k_resultFormatType);
}

void DisplayModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  /* Number of significants figure row */
  if (index == numberOfRows()-1) {
    MessageTableCellWithEditableTextWithSeparator * myCell = (MessageTableCellWithEditableTextWithSeparator *)cell;
    GenericSubController::willDisplayCellForIndex(myCell->messageTableCellWithEditableText(), index);
    constexpr int bufferSize = 3;
    char buffer[bufferSize];
    Integer(Preferences::sharedPreferences()->numberOfSignificantDigits()).serialize(buffer, bufferSize);
    myCell->messageTableCellWithEditableText()->setAccessoryText(buffer);
    return;
  }
  PreferencesController::willDisplayCellForIndex(cell, index);
}

bool DisplayModeController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool DisplayModeController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  if (floatBody < 1.0) {
   floatBody = 1.0;
  }
  if (Preferences::sharedPreferences()->displayMode() == Preferences::PrintFloatMode::Engineering && floatBody < 3.0) {
    floatBody = 3.0;
  }
  if (floatBody > PrintFloat::k_numberOfStoredSignificantDigits) {
    floatBody = PrintFloat::k_numberOfStoredSignificantDigits;
  }
  Preferences::sharedPreferences()->setNumberOfSignificantDigits((char)std::round(floatBody));
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  if (event == Ion::Events::Up || event == Ion::Events::OK) {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

}

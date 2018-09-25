#include "display_mode_controller.h"
#include "../../shared/poincare_helpers.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Settings {

DisplayModeController::DisplayModeController(Responder * parentResponder) :
  PreferencesController(parentResponder),
  m_editableCell(&m_selectableTableView, this, m_draftTextBuffer)
{
  m_editableCell.messageTableCellWithEditableText()->setMessage(I18n::Message::SignificantFigures);
  m_editableCell.messageTableCellWithEditableText()->setMessageFontSize(KDText::FontSize::Large);
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
  if (type == 1) {
    assert(index == 0);
    return &m_editableCell;
  }
  return PreferencesController::reusableCell(index, type);
}

int DisplayModeController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return PreferencesController::k_totalNumberOfCell;
    case 1:
      return 1;
    default:
      assert(false);
      return 0;
  }
}

int DisplayModeController::typeAtLocation(int i, int j) {
  return (j == numberOfRows() - 1 ? 1 : 0);
}

void DisplayModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  /* Number of significants figure row */
  if (index == numberOfRows()-1) {
    MessageTableCellWithEditableTextWithSeparator * myCell = (MessageTableCellWithEditableTextWithSeparator *)cell;
    GenericSubController::willDisplayCellForIndex(myCell->messageTableCellWithEditableText(), index);
    char buffer[3];
    Integer(Preferences::sharedPreferences()->numberOfSignificantDigits()).serialize(buffer, 3);
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
  Context * globalContext = textFieldDelegateApp()->localContext();
  float floatBody = PoincareHelpers::ApproximateToScalar<float>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    floatBody = PrintFloat::k_numberOfPrintedSignificantDigits;
  }
  if (floatBody < 1) {
   floatBody = 1;
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

Shared::TextFieldDelegateApp * DisplayModeController::textFieldDelegateApp() {
  return (Shared::TextFieldDelegateApp *)app();
}

}

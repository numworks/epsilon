#include "math_options_controller.h"
#include <assert.h>
#include "../../global_preferences.h"

using namespace Poincare;

namespace Settings {

MathOptionsController::MathOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  GenericSubController(parentResponder),
  m_preferencesController(this),
  m_displayModeController(this, inputEventHandlerDelegate)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool MathOptionsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    GenericSubController * subController = nullptr;
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::DisplayMode)
      subController = &m_displayModeController;
    else
      subController = &m_preferencesController;
    subController->setMessageTreeModel(m_messageTreeModel->children(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(subController);
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * MathOptionsController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int MathOptionsController::reusableCellCount(int type) {
  assert(type == 0);
  return k_totalNumberOfCell;
}

void MathOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  Preferences * preferences = Preferences::sharedPreferences();
  I18n::Message thisLabel = m_messageTreeModel->children(index)->label();
  myTextCell->setMessage(thisLabel);

  //add text for preferences
  int childIndex = -1;
  switch (thisLabel) {
    case I18n::Message::AngleUnit:
      childIndex = (int)preferences->angleUnit();
      break;
    case I18n::Message::DisplayMode:
      childIndex = (int)preferences->displayMode();
      break;
    case I18n::Message::EditionMode:
      childIndex = (int)preferences->editionMode();
      break;
    case I18n::Message::ComplexFormat:
      childIndex = (int)preferences->complexFormat();
      break;
    case I18n::Message::SymbolMultiplication:
      childIndex = (int)preferences->symbolofMultiplication();
      break;
    case I18n::Message::ResultDisplay:
      childIndex = (int)preferences->resultDisplay();
    case I18n::Message::SymbolFunction:
      childIndex = (int)preferences->symbolofFunction();
      break;      
    default:
      break;
  }
  I18n::Message message = childIndex >= 0 ? m_messageTreeModel->children(index)->children(childIndex)->label() : I18n::Message::Default;
  myTextCell->setSubtitle(message);
}

}

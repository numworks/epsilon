#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>
#include <poincare/preferences.h>
#include <apps/i18n.h>
#include <apps/settings/main_controller.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_preferencesController(this)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void ExamModeController::didEnterResponderChain(Responder * previousFirstResponder) {
  m_selectableTableView.reloadData();
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  I18n::Message childLabel = m_messageTreeModel->children(selectedRow())->label();
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (childLabel == I18n::Message::ActivateExamMode || childLabel == I18n::Message::ExamModeActive) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
        return false;
      }
      AppsContainer::sharedAppsContainer()->displayExamModePopUp(true);
      return true;
    }
    if (childLabel == I18n::Message::LEDColor) {
      GenericSubController * subController = &m_preferencesController;
      subController->setMessageTreeModel(m_messageTreeModel->children(selectedRow()));
      StackViewController * stack = stackController();
      stack->push(subController);
      return true;
    }
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int ExamModeController::reusableCellCount(int type) {
  return k_totalNumberOfCell;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Preferences * preferences = Preferences::sharedPreferences();
  GenericSubController::willDisplayCellForIndex(cell, index);
  I18n::Message thisLabel = m_messageTreeModel->children(index)->label();

  if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate && (thisLabel == I18n::Message::ActivateExamMode || thisLabel == I18n::Message::ExamModeActive)) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
  if (thisLabel == I18n::Message::LEDColor) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    I18n::Message message = m_messageTreeModel->children(ledIndex)->children((int)preferences->colorOfLED())->label();
    myTextCell->setSubtitle(message);
  }
}

}

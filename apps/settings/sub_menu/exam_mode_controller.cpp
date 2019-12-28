#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>
#include <poincare/preferences.h>
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_preferencesController(this),
  m_examModeCell(I18n::Message::Default, KDFont::LargeFont),
  m_ledCell(KDFont::LargeFont, KDFont::SmallFont),
  m_modeCell(KDFont::LargeFont, KDFont::SmallFont)
{
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
I18n::Message childLabel = m_messageTreeModel->children(selectedRow())->label();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
      // If the exam mode is already on, this re-activate the same exam mode
      GlobalPreferences::ExamMode mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
      if (childLabel == I18n::Message::ActivateExamMode || childLabel == I18n::Message::ExamModeActive)
        AppsContainer::sharedAppsContainer()->displayExamModePopUp(mode);
      return true;
    }
    
    if (childLabel == I18n::Message::ActivateExamMode || childLabel == I18n::Message::ExamModeActive) {
      GlobalPreferences::ExamMode mode = GlobalPreferences::ExamMode::Standard;
      AppsContainer::sharedAppsContainer()->displayExamModePopUp(GlobalPreferences::sharedGlobalPreferences()->tempExamMode());
      return true;
    }
    if (childLabel == I18n::Message::LEDColor || childLabel == I18n::Message::ExamModeMode) {
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
  assert(index >= 0  && index < 3);
  
  switch(index) {
    case 0:
      return &m_ledCell;
    case 1:
      return &m_modeCell;
    case 2:
      return &m_examModeCell;
  }
}

int ExamModeController::reusableCellCount(int type) {
  assert(type == 0);
  return 3;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Preferences * preferences = Preferences::sharedPreferences();
  GenericSubController::willDisplayCellForIndex(cell, index);
  I18n::Message thisLabel = m_messageTreeModel->children(index)->label();

  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode() && (thisLabel == I18n::Message::ActivateExamMode || thisLabel == I18n::Message::ExamModeActive)) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
  if (thisLabel == I18n::Message::LEDColor) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    I18n::Message message = (I18n::Message) m_messageTreeModel->children(index)->children((int)preferences->colorOfLED())->label();
    myTextCell->setSubtitle(message);
  }
  if (thisLabel == I18n::Message::ExamModeMode) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    I18n::Message message = (I18n::Message) m_messageTreeModel->children(index)->children((uint8_t)GlobalPreferences::sharedGlobalPreferences()->tempExamMode() - 1)->label();
    myTextCell->setSubtitle(message);
  }
}

}

#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
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
  m_ledCell(KDFont::LargeFont, KDFont::SmallFont)
{
}

void ExamModeController::didEnterResponderChain(Responder * previousFirstResponder) {
  m_selectableTableView.reloadData();
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  I18n::Message childLabel = m_messageTreeModel->children(selectedRow())->label();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
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
  assert(index == 0);
  if (type == 0) {
    return &m_ledCell;
  }
  return &m_examModeCell;
}

int ExamModeController::reusableCellCount(int type) {
  switch (type) {
    case 0:
      return 1;
    case 1:
      return 1;
    default:
      assert(false);
      return 0;
  }
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
    I18n::Message message = (I18n::Message) m_messageTreeModel->children(index)->children((int)preferences->colorOfLED())->label();
    myTextCell->setSubtitle(message);
  }
}

int ExamModeController::typeAtLocation(int i, int j) {
  switch (j) {
    case 0:
      return 0;
    case 1:
      return 1;
    default:
      assert(false);
      return 0;
  }
}

}

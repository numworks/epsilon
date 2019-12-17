#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_cell{MessageTableCell(I18n::Message::ExamModeActive, KDFont::LargeFont), MessageTableCell(I18n::Message::ActivateDutchExamMode, KDFont::LargeFont)}
{
}

int ExamModeController::initialSelectedRow() const {
  int row = selectedRow();
  if (row >= numberOfRows()) {
    return numberOfRows()-1;
  } else if (row < 0) {
    return 0;
  }
  return row;
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    GlobalPreferences::ExamMode mode = GlobalPreferences::ExamMode::Standard;
    if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
      // If the exam mode is already on, this re-activate the same exam mode
      mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
    } else if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::ActivateDutchExamMode) {
      mode = GlobalPreferences::ExamMode::Dutch;
    }
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(mode);
    return true;
  }
  return GenericSubController::handleEvent(event);
}

int ExamModeController::numberOfRows() const {
  if (GlobalPreferences::sharedGlobalPreferences()->language() != I18n::Language::EN || GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    return 1;
  }
  return GenericSubController::numberOfRows();
}

HighlightCell * ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_maxNumberOfCells);
  return &m_cell[index];
}

int ExamModeController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
}

}

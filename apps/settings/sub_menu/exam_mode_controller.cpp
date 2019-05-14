#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_cell(I18n::Message::ExamModeActive, KDFont::LargeFont)
{
}

void ExamModeController::didEnterResponderChain(Responder * previousFirstResponder) {
  m_selectableTableView.reloadData();
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
      return false;
    }
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(true);
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index == 0);
  return &m_cell;
}

int ExamModeController::reusableCellCount(int type) {
  return 1;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
}

}

#include "exam_mode_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

int ExamModeController::ContentView::numberOfCautionLines() const {
  return 0;
}

GlobalPreferences::ExamMode ExamModeController::examMode() {
  GlobalPreferences::ExamMode mode = GlobalPreferences::ExamMode::Standard;
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    // If the exam mode is already on, this re-activate the same exam mode
    mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  } else if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::ActivateDutchExamMode) {
    mode = GlobalPreferences::ExamMode::Dutch;
  }
  return mode;
}

int ExamModeController::numberOfRows() const {
  if (GlobalPreferences::sharedGlobalPreferences()->language() != I18n::Language::EN || GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    return 1;
  }
  return GenericSubController::numberOfRows();
}

}

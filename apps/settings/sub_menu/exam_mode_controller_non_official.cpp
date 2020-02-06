#include "exam_mode_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

int ExamModeController::ContentView::numberOfCautionLines() const {
  return k_maxNumberOfLines;
}

GlobalPreferences::ExamMode ExamModeController::examMode() {
  return GlobalPreferences::ExamMode::Standard;
}

int ExamModeController::numberOfRows() const {
  return 1;
}

}

#include "exam_mode_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

GlobalPreferences::ExamMode ExamModeController::examMode() {
  return GlobalPreferences::ExamMode::Standard;
}

int ExamModeController::numberOfRows() const {
  return 1;
}

int ExamModeController::numberOfCautionLines() const {
  return k_numberOfCautionMessageLines;
}

}

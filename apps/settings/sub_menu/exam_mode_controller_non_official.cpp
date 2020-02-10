#include "exam_mode_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

int ExamModeController::numberOfCautionLines() const {
  return k_numberOfCautionMessageLines;
}

}

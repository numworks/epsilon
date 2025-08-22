#pragma once

#include "context.h"
#include "exam_mode.h"
#include "preferences.h"

namespace Poincare {

void Init(Preferences::Interface* preferences = nullptr,
          const ExamMode* examModePtr = nullptr,
          const Poincare::Context* globalContext = nullptr);
void Shutdown();

}  // namespace Poincare

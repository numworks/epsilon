#ifndef POINCARE_INIT_H
#define POINCARE_INIT_H

#include "exam_mode.h"
#include "preferences.h"

namespace Poincare {

void Init(Preferences::PartialInterface* prefs = nullptr,
          const ExamMode* examModePtr = nullptr);
void Shutdown();

}  // namespace Poincare

#endif

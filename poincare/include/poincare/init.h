#ifndef POINCARE_INIT_H
#define POINCARE_INIT_H

#include "context.h"
#include "exam_mode.h"
#include "preferences.h"

namespace Poincare {

void Init(Preferences::Interface* preferences = nullptr,
          const ExamMode* examModePtr = nullptr,
          Poincare::Context* globalContext = nullptr);
void Shutdown();

}  // namespace Poincare

#endif

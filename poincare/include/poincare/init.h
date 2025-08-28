#pragma once

#include <poincare/sequence_approximation_helper.h>

#include "exam_mode.h"
#include "preferences.h"

namespace Poincare {

void Init(Preferences::Interface* preferences = nullptr,
          const ExamMode* examModePtr = nullptr,
          const SequenceApproximationHelper::ContextInterface* sequenceContext =
              nullptr);
void Shutdown();

}  // namespace Poincare

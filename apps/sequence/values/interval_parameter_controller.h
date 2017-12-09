#ifndef SEQUENCE_INTERVAL_PARAM_CONTROLLER_H
#define SEQUENCE_INTERVAL_PARAM_CONTROLLER_H

#include "../../shared/interval_parameter_controller.h"

namespace Sequence {

class IntervalParameterController final : public Shared::IntervalParameterController {
public:
  using Shared::IntervalParameterController::IntervalParameterController;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
};

}

#endif


#pragma once

#include <omg/global_box.h>
#include <poincare/sequence_approximation_helper.h>

#include "../helper.h"
#include "omg/unreachable.h"

namespace Poincare {

class ForbiddenSequenceContext final
    : public SequenceApproximationHelper::ContextInterface {
 public:
  static OMG::GlobalBox<ForbiddenSequenceContext> s_instance;

  double approximateSequenceAtRank(const char* identifier,
                                   int rank) const override {
    quiz_assert_print_if_failure(false,
                                 "Sequence approximation is application "
                                 "dependent, move tests to the right project.");
    OMG_UNREACHABLE;
  }
};

}  // namespace Poincare

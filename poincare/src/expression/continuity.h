#ifndef POINCARE_EXPRESSION_CONTINUITY_H
#define POINCARE_EXPRESSION_CONTINUITY_H

#include <poincare/src/memory/tree.h>

#include "variables.h"

namespace Poincare::Internal {

class Continuity {
 public:
  /* These functions only return true if the discontinuity is not asymptotic
   * (i.e. for the functions random, randint, round, floor and ceil).
   * Functions like 1/x are not handled here since it "obvious" that they are
   * discontinuous.
   * They also return true for continuous functions like Abs that are tricky for
   * the numerical algorithms.
   */

  static bool InvolvesDiscontinuousFunction(const Tree* e) {
    return e->hasDescendantSatisfying(ShallowIsDiscontinuous);
  }

  // e must be a system function
  static bool IsDiscontinuousBetweenFloatValues(const Tree* e, float x1,
                                                float x2);

 private:
  static bool ShallowIsDiscontinuous(const Tree* e);
};

}  // namespace Poincare::Internal
#endif

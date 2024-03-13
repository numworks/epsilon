#ifndef POINCARE_EXPRESSION_CONTINUITY_H
#define POINCARE_EXPRESSION_CONTINUITY_H

#include <poincare_junior/src/memory/tree.h>

#include "variables.h"

namespace PoincareJ {

class Continuity {
 public:
  /* These functions only return true if the discontinuity is not asymptotic
   * (i.e. for the functions random, randint, round, floor and ceil).
   * Functions like 1/x are not handled here since it "obvious" that they are
   * discontinuous.
   * They also return true for continuous functions like Abs that are tricky for
   * the numerical algorithms.
   */

  static bool InvolvesDiscontinuousFunction(const Tree *e) {
    return e->matchInSelfAndDescendants(ShallowIsDiscontinuous);
  }

  static bool IsDiscontinuousBetweenValuesForSymbol(
      const Tree *e, const char *symbol, float x1, float x2
      /*const ApproximationContext &approximationContext*/);

 private:
  static bool ShallowIsDiscontinuous(const Tree *e);
};

}  // namespace PoincareJ
#endif

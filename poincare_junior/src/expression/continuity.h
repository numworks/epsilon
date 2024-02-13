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
   * discontinuous. */

  static bool IsDiscontinuous(const Tree *e) {
    return e->isRandomNode() || e->isPiecewise() ||
           (e->isOfType({BlockType::Floor, BlockType::Round, BlockType::Ceiling,
                         BlockType::FracPart, BlockType::Abs}) &&
            Variables::HasVariables(e));
  };

  static bool InvolvesDiscontinuousFunction(const Tree *e) {
    return e->recursivelyMatches(IsDiscontinuous);
  }

  static bool IsDiscontinuousBetweenValuesForSymbol(
      const Tree *e, const char *symbol, float x1, float x2
      /*const ApproximationContext &approximationContext*/);
};

}  // namespace PoincareJ
#endif

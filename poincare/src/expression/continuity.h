#ifndef POINCARE_EXPRESSION_CONTINUITY_H
#define POINCARE_EXPRESSION_CONTINUITY_H

#include <poincare/src/memory/tree.h>

#include "variables.h"

namespace Poincare::Internal {

/* WARNING: Everywhere in Poincaré, the term "discontinuity" refers to
 * "jump-discontinuity" (i.e. finite discontinuities like piecewise, random,
 * round, floor, ceil, ...).
 * Infinite/asymptotic discontinuities (like 1/x) are
 * not handled when calling IsDiscontinuousOnInterval or
 * InvolvesDiscontinuousFunction (partly because they are more obvious to detect
 * by numerical algorithms).
 *
 * Jump-discontinuity (handled in this file):
 *      o-----
 *
 * -----o
 *
 * Asymptotic discontinuity (not handled in this file):
 *      |
 *      |
 *     /
 * ----    ----
 *        /
 *       |
 *       |
 *
 * WARNING: The Abs function is also considered to have a jump-discontinuity in
 * zero, as it can cause problems in expressions like "|x|/x".
 */

class Continuity {
 public:
  static bool InvolvesDiscontinuousFunction(const Tree* e) {
    return e->hasDescendantSatisfying(ShallowIsDiscontinuous);
  }

  // e must be a system function
  template <typename T>
  static bool IsDiscontinuousOnInterval(const Tree* e, T minBound, T maxBound);

 private:
  static bool ShallowIsDiscontinuous(const Tree* e);
};

}  // namespace Poincare::Internal
#endif

#pragma once

#include <poincare/src/memory/tree.h>

#include "approximation.h"

namespace Poincare::Internal {

/* WARNING: Everywhere in Poincaré, we handled different types of discontinuity
 differently.
 *
 * Jump-discontinuity:
 *  - piecewise, random, round, floor, ceil, ...
 *  - Handled in InvolvesDiscontinuousFunction and IsDiscontinuousOnInterval:
 *
 *      o-----
 *
 * -----o
 *
 * Singular discontinuity:
 *  - piecewise, NonNull
 *  - Handled in IsDiscontinuousOnInterval
 *
 * -----o-----
 *
 * Asymptotic discontinuity:
 *  - 1/x, x^-2, ln(x), ...
 *  - More obvious to detect by numerical algorithms
 *  - Often handled in IsDiscontinuousOnInterval anyway because of cases like
 *    f(x)/x where f closely ressemble x around 0, like "|x|/x" or "sin(x)/x".
 *
 *      |
 *      |
 *     /
 * ----    ----
 *        /
 *       |
 *       |
 *
 * IsDiscontinuousOnInterval method handles discontinuity on an interval. Bigger
 * intervals are more likely to return false negatives because only the bounds
 * are tested to improve performances.
 *
 * Here are three examples, within the [start, end] interval:
 *      start  end
 *        |     |
 * ---------o   |   o-----
 *        |     |             Discontinuity is detected
 *        | o-------o
 *        |     |
 *        |     |
 * ---------o o--------
 *        |     |             Discontinuity is not detected
 *        | o-o |
 *        |     |
 *        |     |
 * -----o |     | o-----     Considered continuous for better performances
 *
 * False negative can lead to inaccurate plot or solver solutions.
 * False positives can cause performances issues. */

class Continuity {
 public:
  static bool InvolvesDiscontinuousFunction(const Tree* e) {
    return e->hasDescendantSatisfying(ShallowIsDiscontinuous);
  }

  // e must be a prepared function
  template <typename T>
  static bool IsDiscontinuousOnInterval(const Tree* e, T minBound, T maxBound);

 private:
  static bool ShallowIsDiscontinuous(const Tree* e);
  /* Return true if e can become undefined on [minBound, maxBound]. If
   * checkContinuity is true, also return if e can be discontinuous. */
  template <typename T>
  static bool CanBeUndefinedOnInterval(const Tree* e, T minBound, T maxBound,
                                       const Approximation::Parameters& params,
                                       bool checkContinuity);
};

}  // namespace Poincare::Internal

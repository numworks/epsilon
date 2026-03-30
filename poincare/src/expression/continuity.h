#pragma once

#include <poincare/src/memory/tree.h>

#include "approximation.h"

namespace Poincare::Internal {

/* WARNING: Everywhere in Poincaré, the term "discontinuity" refers to
 * "jump-discontinuity" (i.e. finite discontinuities like piecewise, random,
 * round, floor, ceil, ...) or "singular discontinuity" (i.e undefined value in
 * a single point like (x_1)/(x-1) or piecewise). Most Infinite/asymptotic
 * discontinuities (like ln(x)) are not handled when calling
 * IsDiscontinuousOnInterval or InvolvesDiscontinuousFunction (partly because
 * they are more obvious to detect by numerical algorithms).
 *
 * Jump-discontinuity (handled in this file):
 *      o-----
 *
 * -----o
 *
 * Singular discontinuity (handled in this file):
 * -----o-----
 *
 * Asymptotic discontinuity (not always handled in this file):
 *      |
 *      |
 *     /
 * ----    ----
 *        /
 *       |
 *       |
 *
 * WARNING: Asymptotic discontinuity with 1/x or x^-2 is handled here because of
 * cases like f(x)/x where f closely ressemble x around 0, like "|x|/x" or
 * "sin(x)/x". */

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
  /* Return true if e can be undefined on [minBound, maxBound]. If
   * checkContinuity is true, also return if e can be discontinuous. */
  template <typename T>
  static bool CanBeUndefinedOnInterval(const Tree* e, T minBound, T maxBound,
                                       const Approximation::Parameters& params,
                                       bool checkContinuity);
};

}  // namespace Poincare::Internal

#include "continuity.h"

#include "approximation.h"
#include "undefined.h"
#include "variables.h"

namespace Poincare::Internal {

bool Continuity::ShallowIsDiscontinuous(const Tree* e) {
  /* TODO: Add Abs, Div, NonNull and negative powers while preserving
   * performances. */
  return e->isRandomized() ||
         (e->isOfType({Type::Piecewise, Type::Floor, Type::Round, Type::Ceil,
                       Type::Frac, Type::Sign}) &&
          Variables::HasVariables(e));
};

template <typename T>
bool CanBeNullOnInterval(const Tree* e, T minBound, T maxBound,
                         const Approximation::Parameters& params) {
  T approximationMin = Approximation::To<T>(e, minBound, params);
  T approximationMax = Approximation::To<T>(e, maxBound, params);
  // Check for null or sign change
  // use !(==) instead of != to consider NaN as a sign change
  return (approximationMin == 0.0) || (approximationMax == 0.0) ||
         !((approximationMin > 0.0) == (approximationMax > 0.0));
}

template <typename T>
bool CanChangeValues(const Tree* e, T minBound, T maxBound,
                     const Approximation::Parameters& params) {
  // TODO: Should check across the interval
  // use !(==) instead of != to consider NaN as a value change
  return !(Approximation::To<T>(e, minBound, params) ==
           Approximation::To<T>(e, maxBound, params));
}

template <typename T>
bool Continuity::IsDiscontinuousOnInterval(const Tree* e, T minBound,
                                           T maxBound) {
  Approximation::Parameters params{
      .policy = {.optimize = true},
      .expressionParameters = Approximation::Prepared{}};
  return CanBeUndefinedOnInterval(e, minBound, maxBound, params, true);
}

template <typename T>
bool Continuity::CanBeUndefinedOnInterval(
    const Tree* e, T minBound, T maxBound,
    const Approximation::Parameters& params, bool checkContinuity) {
  if (checkContinuity) {
    /* Discontinuity checks */
    if (e->isRandomized()) {
      return true;
    }
    if (e->isOfType(
            {Type::Ceil, Type::Floor, Type::Frac, Type::Round, Type::Sign}) &&
        CanChangeValues(e, minBound, maxBound, params)) {
      return true;
    }
  } else if (!Undefined::CanBeUndefined(e)) {
    return false;
  }
  /* Undefined checks */
  if (e->isOfType({Type::Sign, Type::Div, Type::NonNull}) ||
      (e->isPow() && e->child(1)->isNegativeRational())) {
    int childIndex = e->isDiv() ? 1 : 0;
    if (CanBeNullOnInterval(e->child(childIndex), minBound, maxBound, params)) {
      return true;
    }
  }
  if (e->isPiecewise()) {
    return CanPiecewiseBeUndefinedOnInterval(e, minBound, maxBound, params,
                                             checkContinuity);
  }
  for (IndexedChild indexedChild : e->indexedChildren()) {
    // Skip discontinuity check in DepLists.
    if (CanBeUndefinedOnInterval(indexedChild, minBound, maxBound, params,
                                 checkContinuity && !e->isDepList())) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool Continuity::CanPiecewiseBeUndefinedOnInterval(
    const Tree* e, T minBound, T maxBound,
    const Approximation::Parameters& params, bool checkContinuity) {
  // Get the piecewise indexes at bounds of the interval
  int minIndex = Approximation::IndexOfActivePiecewiseBranchAt(e, minBound);
  int maxIndex = Approximation::IndexOfActivePiecewiseBranchAt(e, maxBound);

  bool hasUnknownBranch = minIndex == Approximation::k_unknownActiveBranch ||
                          maxIndex == Approximation::k_unknownActiveBranch;
  if (checkContinuity && (hasUnknownBranch || minIndex != maxIndex)) {
    // Any active branch change is a discontinuity
    return true;
  }
  bool hasNoActiveBranch = minIndex == Approximation::k_noActiveBranch ||
                           maxIndex == Approximation::k_noActiveBranch;
  if (hasNoActiveBranch) {
    // Ignore when piecewise is inactive at both end of the interval
    return minIndex != maxIndex;
  }
  assert(!hasNoActiveBranch);
  for (IndexedChild indexedChild : e->indexedChildren()) {
    bool isCondition = (indexedChild.index % 2 == 1);
    int branchIndex = isCondition ? indexedChild.index - 1 : indexedChild.index;
    bool checkChild =
        // Check everything if branch is Unknown
        hasUnknownBranch ||
        // Always check the values and conditions on the interval bounds
        (branchIndex == minIndex || branchIndex == maxIndex) ||
        // Always check previous conditions, since they will be evaluated
        (isCondition && (branchIndex < minIndex || branchIndex < maxIndex));

    /* Note: We could skip continuity check in conditions, but it would lead to
     * more false negatives. */
    if (checkChild && CanBeUndefinedOnInterval(indexedChild, minBound, maxBound,
                                               params, checkContinuity)) {
      return true;
    }
  }
  return false;
}

template bool Continuity::IsDiscontinuousOnInterval(const Tree*, float, float);
template bool Continuity::IsDiscontinuousOnInterval(const Tree*, double,
                                                    double);

template bool Continuity::CanBeUndefinedOnInterval(
    const Tree*, float, float, const Approximation::Parameters&, bool);
template bool Continuity::CanBeUndefinedOnInterval(
    const Tree*, double, double, const Approximation::Parameters&, bool);

template bool Continuity::CanPiecewiseBeUndefinedOnInterval(
    const Tree*, float, float, const Approximation::Parameters&, bool);
template bool Continuity::CanPiecewiseBeUndefinedOnInterval(
    const Tree*, double, double, const Approximation::Parameters&, bool);

}  // namespace Poincare::Internal

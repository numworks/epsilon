#include "continuity.h"

#include "approximation.h"
#include "undefined.h"
#include "variables.h"

namespace Poincare::Internal {

bool Continuity::ShallowIsDiscontinuous(const Tree* e) {
  // NOTE: With piecewise, variables could only be looked for in conditions
  return e->isRandomized() ||
         ((e->isOfType({Type::Piecewise, Type::Floor, Type::Round, Type::Ceil,
                        Type::Frac, Type::Abs, Type::Sign, Type::Div,
                        Type::NonNull}) ||
           (e->isPow() && e->child(1)->isNegativeRational())) &&
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
bool CanChangeActivePiecewiseBranch(const Tree* e, T minBound, T maxBound) {
  // TODO: Should check across the interval
  return Approximation::IndexOfActivePiecewiseBranchAt(e, minBound) !=
         Approximation::IndexOfActivePiecewiseBranchAt(e, maxBound);
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
    if (e->isPiecewise() &&
        CanChangeActivePiecewiseBranch(e, minBound, maxBound)) {
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
  for (IndexedChild indexedChild : e->indexedChildren()) {
    // Skip discontinuity check in DepLists.
    if (CanBeUndefinedOnInterval(indexedChild, minBound, maxBound, params,
                                 checkContinuity && !e->isDepList())) {
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

}  // namespace Poincare::Internal

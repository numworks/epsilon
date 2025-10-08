#include "continuity.h"

#include "approximation.h"

namespace Poincare::Internal {

bool Continuity::ShallowIsDiscontinuous(const Tree* e) {
  return e->isRandomized() || e->isPiecewise() ||
         (e->isOfType({Type::Floor, Type::Round, Type::Ceil, Type::Frac,
                       Type::Abs, Type::Sign}) &&
          Variables::HasVariables(e));
};

template <typename T>
bool Continuity::IsDiscontinuousOnInterval(const Tree* e, T minBound,
                                           T maxBound) {
  // TODO_PCJ: symbol is ignored for now
  if (e->isRandomized()) {
    return true;
  }
  bool isDiscontinuous = false;
  Approximation::Parameters params{};
  if (e->isOfType({Type::Ceil, Type::Floor, Type::Round})) {
    // is discontinuous if it changes value
    isDiscontinuous = Approximation::To<T>(e, minBound, params) !=
                      Approximation::To<T>(e, maxBound, params);
  } else if (e->isFrac()) {
    // is discontinuous if the child changes int value
    isDiscontinuous =
        std::floor(Approximation::To<T>(e->child(0), minBound, params)) !=
        std::floor(Approximation::To<T>(e->child(0), maxBound, params));
  } else if (e->isOfType({Type::Sign, Type::Div}) ||
             (e->isPow() && e->child(1)->isNegativeInteger())) {
    int childIndex = e->isDiv() ? 1 : 0;
    // is discontinuous if the child changes sign
    isDiscontinuous =
        (Approximation::To<T>(e->child(childIndex), minBound, params) > 0.0) !=
        (Approximation::To<T>(e->child(childIndex), maxBound, params) > 0.0);
  } else if (e->isPiecewise()) {
    int index = Approximation::IndexOfActivePiecewiseBranchAt(e, minBound);
    assert(index < 0 || index < e->numberOfChildren());
    /* NOTE Consider e discontinuous if active branch either :
     * 1. Approximated to undef
     * 2. Is different at each bounds
     * 3. Is discontinuous */
    if (index == -2 ||
        index != Approximation::IndexOfActivePiecewiseBranchAt(e, maxBound) ||
        (index >= 0 &&
         IsDiscontinuousOnInterval(e->child(index), minBound, maxBound))) {
      return true;
    }
    // Handle piecewise condition discontinuity
    for (IndexedChild indexedChild : e->indexedChildren()) {
      if (index >= 0 && indexedChild.index > index + 1) {
        // None of the previous conditions where discontinuous
        break;
      }
      if (indexedChild.index % 2 == 0) {
        // Skip piecewise branches
        continue;
      }
      if (IsDiscontinuousOnInterval(indexedChild, minBound, maxBound)) {
        // Another branch could be active within the interval
        return true;
      }
    }
    // TODO: Even if continuous, active branch could change within the interval
    return false;
  }
  if (isDiscontinuous) {
    return true;
  }
  for (const Tree* child : e->children()) {
    if (IsDiscontinuousOnInterval(child, minBound, maxBound)) {
      return true;
    }
  }
  return false;
}

template bool Continuity::IsDiscontinuousOnInterval(const Tree* e,
                                                    float minBound,
                                                    float maxBound);
template bool Continuity::IsDiscontinuousOnInterval(const Tree* e,
                                                    double minBound,
                                                    double maxBound);

}  // namespace Poincare::Internal

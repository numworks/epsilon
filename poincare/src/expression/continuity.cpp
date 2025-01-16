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
  } else if (e->isOfType({Type::Abs, Type::Sign})) {
    // is discontinuous if the child changes sign
    isDiscontinuous =
        (Approximation::To<T>(e->child(0), minBound, params) > 0.0) !=
        (Approximation::To<T>(e->child(0), maxBound, params) > 0.0);
  } else if (e->isPiecewise()) {
    isDiscontinuous =
        Approximation::IndexOfActivePiecewiseBranchAt(e, minBound) !=
        Approximation::IndexOfActivePiecewiseBranchAt(e, maxBound);
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

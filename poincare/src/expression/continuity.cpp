#include "continuity.h"

#include "approximation.h"

namespace Poincare::Internal {

bool Continuity::ShallowIsDiscontinuous(const Tree* e) {
  return e->isRandomized() || e->isPiecewise() ||
         (e->isOfType(
              {Type::Floor, Type::Round, Type::Ceil, Type::Frac, Type::Abs}) &&
          Variables::HasVariables(e));
};

template <typename T>
bool Continuity::IsDiscontinuousBetweenValues(const Tree* e, T x1, T x2) {
  // TODO_PCJ: symbol is ignored for now
  if (e->isRandomized()) {
    return true;
  }
  bool isDiscontinuous = false;
  if (e->isOfType({Type::Ceil, Type::Floor, Type::Round})) {
    // is discontinuous if it changes value
    isDiscontinuous = Approximation::RootTreeToReal<T>(e, x1) !=
                      Approximation::RootTreeToReal<T>(e, x2);
  } else if (e->isFrac()) {
    // is discontinuous if the child changes int value
    isDiscontinuous =
        std::floor(Approximation::RootTreeToReal<T>(e->child(0), x1)) !=
        std::floor(Approximation::RootTreeToReal<T>(e->child(0), x2));
  } else if (e->isOfType({Type::Abs, Type::Sign})) {
    // is discontinuous if the child changes sign
    isDiscontinuous =
        (Approximation::RootTreeToReal<T>(e->child(0), x1) > 0.0) !=
        (Approximation::RootTreeToReal<T>(e->child(0), x2) > 0.0);
  } else if (e->isPiecewise()) {
    isDiscontinuous = Approximation::IndexOfActivePiecewiseBranchAt(e, x1) !=
                      Approximation::IndexOfActivePiecewiseBranchAt(e, x2);
  }
  if (isDiscontinuous) {
    return true;
  }
  for (const Tree* child : e->children()) {
    if (IsDiscontinuousBetweenValues(child, x1, x2)) {
      return true;
    }
  }
  return false;
}

template bool Continuity::IsDiscontinuousBetweenValues(const Tree* e, float x1,
                                                       float x2);
template bool Continuity::IsDiscontinuousBetweenValues(const Tree* e, double x1,
                                                       double x2);

}  // namespace Poincare::Internal

#include "continuity.h"

#include "approximation.h"

namespace PoincareJ {

bool Continuity::ShallowIsDiscontinuous(const Tree *e) {
  return e->isRandomNode() || e->isPiecewise() ||
         (e->isOfType({Type::Floor, Type::Round, Type::Ceiling, Type::FracPart,
                       Type::Abs}) &&
          Variables::HasVariables(e));
};

bool Continuity::IsDiscontinuousBetweenValuesForSymbol(const Tree *e,
                                                       const char *symbol,
                                                       float x1, float x2) {
  // TODO PCJ: symbol is ignored for now
  if (e->isRandomNode()) {
    return true;
  }
  bool isDiscontinuous = false;
  if (e->isOfType({Type::Ceiling, Type::Floor, Type::Round})) {
    // is discontinuous if it changes value
    isDiscontinuous =
        Approximation::To<float>(e, x1) != Approximation::To<float>(e, x2);
  } else if (e->isFracPart()) {
    // is discontinuous if the child changes int value
    isDiscontinuous = std::floor(Approximation::To<float>(e->child(0), x1)) !=
                      std::floor(Approximation::To<float>(e->child(0), x2));
  } else if (e->isOfType({Type::Abs, Type::Sign})) {
    // is discontinuous if the child changes sign
    isDiscontinuous = (Approximation::To<float>(e->child(0), x1) > 0.0) !=
                      (Approximation::To<float>(e->child(0), x2) > 0.0);
  } else if (e->isPiecewise()) {
    isDiscontinuous = Approximation::IndexOfActivePiecewiseBranchAt(e, x1) !=
                      Approximation::IndexOfActivePiecewiseBranchAt(e, x2);
  }
  if (isDiscontinuous) {
    return true;
  }
  for (const Tree *child : e->children()) {
    if (IsDiscontinuousBetweenValuesForSymbol(child, symbol, x1, x2)) {
      return true;
    }
  }
  return false;
}

}  // namespace PoincareJ

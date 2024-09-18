#ifndef POINCARE_HELPERS_LAYOUT_H
#define POINCARE_HELPERS_LAYOUT_H

#include <kandinsky/size.h>

namespace Poincare::Internal {
class Tree;
}

namespace Poincare::LayoutHelpers {

void MakeRightMostParenthesisTemporary(Internal::Tree* l);

// KRackL(KAbsL("x"_l)) -> KRackL(KAbsL(""_l))
void DeleteChildrenRacks(Internal::Tree* rack);

// Enforce a correct rack/layout structure by merging or inserting racks
void SanitizeRack(Internal::Tree* rack);

bool IsSanitizedRack(const Internal::Tree* rack);

KDSize Point2DSizeGivenChildSize(KDSize childSize);

}  // namespace Poincare::LayoutHelpers

#endif

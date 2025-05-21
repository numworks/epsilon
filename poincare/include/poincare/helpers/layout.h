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

// Turn 2E3 into 2×10^3. TODO: this should be a setting of the layouter
bool TurnEToTenPowerLayout(Internal::Tree* layout, bool linear = false);

}  // namespace Poincare::LayoutHelpers

#endif

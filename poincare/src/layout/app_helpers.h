#ifndef POINCARE_LAYOUT_APP_HELPERS_H
#define POINCARE_LAYOUT_APP_HELPERS_H

#include <poincare/src/memory/tree.h>

namespace Poincare {
class JuniorLayout;
}

namespace Poincare::Internal {
namespace AppHelpers {

void MakeRightMostParenthesisTemporary(Tree* l);

// KRackL(KAbsL("x"_l)) -> KRackL(KAbsL(""_l))
void DeleteChildrenRacks(Tree* rack);

bool ContainsSmallCapitalE(const Tree* rack);

// Enforce a correct rack/layout structure by merging or inserting racks
void SanitizeRack(Tree* rack);

bool IsSanitizedRack(const Tree* rack);

}  // namespace AppHelpers
}  // namespace Poincare::Internal

#endif

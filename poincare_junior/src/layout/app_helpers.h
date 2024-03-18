#ifndef POINCARE_JUNIOR_LAYOUT_APP_HELPERS_H
#define POINCARE_JUNIOR_LAYOUT_APP_HELPERS_H

#include <poincare_junior/src/memory/tree.h>

namespace Poincare {
class JuniorLayout;
}

namespace PoincareJ {
namespace AppHelpers {

void MakeRightMostParenthesisTemporary(Tree* tree);

void MakeAdditionImplicit(Poincare::JuniorLayout& layout);

bool ContainsSmallCapitalE(const Tree* rack);

// Enforce a correct rack/layout structure by merging or inserting racks
void SanitizeRack(Tree* rack);

}  // namespace AppHelpers
}  // namespace PoincareJ

#endif

#include "app_helpers.h"

#include "autocompleted_pair.h"

namespace PoincareJ {
namespace AppHelpers {

void MakeRightMostParenthesisTemporary(Tree* tree) {
  if (!tree->isRackLayout() || tree->numberOfChildren() == 0) {
    return;
  }
  Tree* lastChild = tree->child(tree->numberOfChildren() - 1);
  if (lastChild->isParenthesisLayout() &&
      !AutocompletedPair::IsTemporary(lastChild, Side::Left)) {
    AutocompletedPair::SetTemporary(lastChild, Side::Right, true);
  }
}

}  // namespace AppHelpers
}  // namespace PoincareJ

#include "float_helper.h"

#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

bool FloatHelper::SetSign(Tree* tree, NonStrictSign sign) {
  double value = To(tree);
  if (value == 0 || (value > 0.0) == (sign == NonStrictSign::Positive)) {
    return false;
  }
  tree->moveTreeOverTree(
      tree->isSingleFloat()
          ? SharedTreeStack->pushSingleFloat(-static_cast<float>(value))
          : SharedTreeStack->pushDoubleFloat(-value));
  return true;
}

}  // namespace Poincare::Internal

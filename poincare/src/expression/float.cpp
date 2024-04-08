#include "float.h"

#include <poincare_junior/src/memory/tree_stack.h>

namespace PoincareJ {

bool FloatNode::SetSign(Tree* tree, OMG::NonStrictSign sign) {
  double value = To(tree);
  if (value == 0 || (value > 0.0) == (sign == OMG::NonStrictSign::Positive)) {
    return false;
  }
  tree->moveTreeOverTree(
      tree->isSingleFloat()
          ? SharedTreeStack->push<Type::SingleFloat>(-static_cast<float>(value))
          : SharedTreeStack->push<Type::DoubleFloat>(-value));
  return true;
}

}  // namespace PoincareJ

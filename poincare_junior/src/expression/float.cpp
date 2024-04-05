#include "float.h"

#include <poincare_junior/src/memory/edition_pool.h>

namespace PoincareJ {

bool FloatNode::SetSign(Tree *tree, NonStrictSign sign) {
  double value = To(tree);
  if (value == 0 || (value > 0.0) == (sign == NonStrictSign::Positive)) {
    return false;
  }
  tree->moveTreeOverTree(
      tree->isSingleFloat()
          ? SharedTreeStack->push<Type::SingleFloat>(-static_cast<float>(value))
          : SharedTreeStack->push<Type::DoubleFloat>(-value));
  return true;
}

}  // namespace PoincareJ

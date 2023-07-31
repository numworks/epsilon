#include "decimal.h"

#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_pool.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

void Decimal::Project(Tree* tree) {
  assertValidDecimal(tree);
  // dec<n>(x) -> 10^(-n)*x
  Tree* mult = SharedEditionPool->push<BlockType::Multiplication>(1);
  SharedEditionPool->push<BlockType::Power>();
  SharedEditionPool->push<BlockType::IntegerShort, int8_t>(10);
  IntegerHandler(DecimalOffset(tree), NonStrictSign::Negative)
      .pushOnEditionPool();
  tree->moveTreeOverNode(mult);
  NAry::SetNumberOfChildren(tree, 2);
}

}  // namespace PoincareJ

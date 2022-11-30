#include "print.h"
#include <poincare_junior/src/memory/tree_constructor.h>
#include <poincare_junior/src/expression/simplification.h>

using namespace Poincare;

void checkComparison(const Node node0, const Node node1, int result) {
  int comparison = Simplification::Compare(node0, node1);
  assert(comparison == result);
}

void testExpressionComparison() {
  checkComparison(10.0_fn, 2.0_fn, 1);
  checkComparison(1.0_fn, 2.0_fn, -1);
  checkComparison(Add(2_sn,u'π'_n), u'π'_n, 1);
  // TODO: complete
}

#include "print.h"
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void assert_comparison_equals(const Node node0, const Node node1, int result) {
  int comparison = Comparison::Compare(node0, node1);
  assert(comparison == result);
}

void testExpressionComparison() {
  assert_comparison_equals(10.0_fn, 2.0_fn, 1);
  assert_comparison_equals(1.0_fn, 2.0_fn, -1);
  assert_comparison_equals(Add(2_sn,u'π'_n), u'π'_n, 1);
  // TODO: complete
}

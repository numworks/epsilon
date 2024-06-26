#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/order.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_comparison_equals(const Tree* node0, const Tree* node1,
                              int result) {
  int comparison = Order::Compare(node0, node1);
  assert(comparison == result);
}

QUIZ_CASE(pcj_expression_comparison) {
  assert_comparison_equals(10.0_fe, 2.0_fe, 1);
  assert_comparison_equals(1.0_fe, 2.0_fe, -1);
  assert_comparison_equals(KAdd(2_e, π_e), π_e, 1);
  // TODO: complete
  SharedTreeStack->flush();
}

void assert_contain_subtree(const Tree* tree, const Tree* subtree) {
  assert(Order::ContainsSubtree(tree, subtree));
}

void assert_not_contain_subtree(const Tree* tree, const Tree* subtree) {
  assert(!Order::ContainsSubtree(tree, subtree));
}

QUIZ_CASE(pcj_subtree) {
  assert_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)),
                         KAdd(2_e, KMult(1_e, 3_e)));
  assert_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)), KMult(1_e, 3_e));
  assert_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)), 3_e);

  assert_not_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)), 4_e);
  assert_not_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)), KMult(1_e, 4_e));
  assert_not_contain_subtree(KAdd(2_e, KMult(1_e, 3_e)),
                             KAdd(2_e, KMult(1_e, 4_e)));
  SharedTreeStack->flush();
}

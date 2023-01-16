#include "print.h"
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void assert_comparison_equals(const Node node0, const Node node1, int result) {
  int comparison = Comparison::Compare(node0, node1);
  assert(comparison == result);
}

void testExpressionComparison() {
  assert_comparison_equals(10.0_n, 2.0_n, 1);
  assert_comparison_equals(1.0_n, 2.0_n, -1);
  assert_comparison_equals(Add("2"_n,u'π'_n), u'π'_n, 1);
  // TODO: complete
  CachePool::sharedCachePool()->editionPool()->flush();
}
QUIZ_CASE(pcj_expression_comparison) { testExpressionComparison(); }

void assert_contain_subtree(const Node tree, const Node subtree) {
  assert(Comparison::ContainsSubtree(tree, subtree));
}

void assert_not_contain_subtree(const Node tree, const Node subtree) {
  assert(!Comparison::ContainsSubtree(tree, subtree));
}

void testSubtree() {
  assert_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), Add("2"_n, Mult("1"_n, "3"_n)));
  assert_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), Mult("1"_n, "3"_n));
  assert_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), "3"_n);

  assert_not_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), "4"_n);
  assert_not_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), Mult("1"_n, "4"_n));
  assert_not_contain_subtree(Add("2"_n, Mult("1"_n, "3"_n)), Add("2"_n, Mult("1"_n, "4"_n)));
  CachePool::sharedCachePool()->editionPool()->flush();
}
QUIZ_CASE(pcj_subtree) { testSubtree(); }

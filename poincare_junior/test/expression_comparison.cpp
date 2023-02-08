#include "print.h"
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void assert_comparison_equals(const Node node0, const Node node1, int result) {
  int comparison = Comparison::Compare(node0, node1);
  assert(comparison == result);
}

QUIZ_CASE(pcj_expression_comparison) {
  assert_comparison_equals(10.0_e, 2.0_e, 1);
  assert_comparison_equals(1.0_e, 2.0_e, -1);
  assert_comparison_equals(Add("2"_e,u'π'_e), u'π'_e, 1);
  // TODO: complete
  CachePool::sharedCachePool()->editionPool()->flush();
}

void assert_contain_subtree(const Node tree, const Node subtree) {
  assert(Comparison::ContainsSubtree(tree, subtree));
}

void assert_not_contain_subtree(const Node tree, const Node subtree) {
  assert(!Comparison::ContainsSubtree(tree, subtree));
}

QUIZ_CASE(pcj_subtree) {
  assert_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), Add("2"_e, Mult("1"_e, "3"_e)));
  assert_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), Mult("1"_e, "3"_e));
  assert_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), "3"_e);

  assert_not_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), "4"_e);
  assert_not_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), Mult("1"_e, "4"_e));
  assert_not_contain_subtree(Add("2"_e, Mult("1"_e, "3"_e)), Add("2"_e, Mult("1"_e, "4"_e)));
  CachePool::sharedCachePool()->editionPool()->flush();
}

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/n_ary.h>

#include "helper.h"

QUIZ_CASE(pcj_compare) {
  QUIZ_ASSERT(Comparison::Compare("a"_e, "a"_e) == 0);
  QUIZ_ASSERT(Comparison::Compare("a"_e, "b"_e) == -1);
  QUIZ_ASSERT(Comparison::Compare("b"_e, "a"_e) == 1);
  QUIZ_ASSERT(Comparison::Compare(2_e, 3_e) == -1);
  QUIZ_ASSERT(Comparison::Compare(KAdd(3_e, "a"_e), KAdd(2_e, "a"_e)) == 1);
  QUIZ_ASSERT(Comparison::Compare(π_e, e_e) == -1);
}

QUIZ_CASE(pcj_compare_for_addition_beautification) {
  Tree *e = KAdd(KVar<0>, KPow(KVar<0>, 2_e), KPow(KVar<0>, KHalf))->clone();
  NAry::Sort(e, Comparison::Order::AdditionBeautification);
  assert_trees_are_equal(
      e, KAdd(KPow(KVar<0>, 2_e), KVar<0>, KPow(KVar<0>, KHalf)));
}

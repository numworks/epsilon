#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>

#include "helper.h"

QUIZ_CASE(pcj_compare) {
  QUIZ_ASSERT(Order::Compare("a"_e, "a"_e) == 0);
  QUIZ_ASSERT(Order::Compare("a"_e, "b"_e) == -1);
  QUIZ_ASSERT(Order::Compare("b"_e, "a"_e) == 1);
  QUIZ_ASSERT(Order::Compare(2_e, 3_e) == -1);
  QUIZ_ASSERT(Order::Compare(KAdd(3_e, "a"_e), KAdd(2_e, "a"_e)) == 1);
  QUIZ_ASSERT(Order::Compare(π_e, e_e) == -1);
}

QUIZ_CASE(pcj_compare_for_addition_beautification) {
  Tree* e = KAdd(KVarX, KPow(KVarX, 2_e), KPow(KVarX, 1_e / 2_e))->cloneTree();
  NAry::Sort(e, Order::OrderType::AdditionBeautification);
  assert_trees_are_equal(e,
                         KAdd(KPow(KVarX, 2_e), KVarX, KPow(KVarX, 1_e / 2_e)));
}

void sorts_to(const char* input, const char* output,
              Order::OrderType order = Order::OrderType::Beautification) {
  static Order::OrderType s_order;
  s_order = order;
  process_tree_and_compare(
      input, output,
      [](Tree* e, ProjectionContext ctx) { NAry::Sort(e, s_order); }, {});
}

QUIZ_CASE(pcj_sort) {
  using enum Order::OrderType;
  sorts_to("π*2*i", "2*π*i");
  // TODO_PCJ: √(2) should be before π and unknowns
  sorts_to("π*2*√(2)*_m", "2×π×√(2)×_m");
  sorts_to("π*2*√(2)*i*a*u", "2×π×a×u×√(2)×i");

  sorts_to("b+b^2+a^2+a^3", "a^3+a^2+b^2+b", AdditionBeautification);
  sorts_to("π+2+i", "2+π+i", AdditionBeautification);
}

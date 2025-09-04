#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/order.h>
#include <poincare/src/memory/n_ary.h>

#include "helper.h"

using namespace Poincare::Internal;
using enum Order::OrderType;

void assert_order_is(const Tree* e1, const Tree* e2, int result = -1,
                     Order::OrderType order = System) {
  int comparison = Order::Compare(e1, e2, order);
  quiz_assert(comparison == result);
}

void assert_order_is(const char* t1, const char* t2, int result = -1,
                     Order::OrderType order = System) {
  Tree* e1 = parse(t1);
  Tree* e2 = parse(t2);
  assert_order_is(e1, e2, result, order);
  e2->removeTree();
  e1->removeTree();
}

QUIZ_CASE(pcj_order) {
  // Numbers
  assert_order_is(2.0_fe, 10.0_fe);
  assert_order_is(1.0_fe, 2.0_fe);
  assert_order_is(0.123_fe, 0.234_fe);
  assert_order_is(-0.2392_de, 0.234_de);
  assert_order_is(123_de, 234_de);
  assert_order_is(1_e, 1_e, 0);
  assert_order_is(2_e, 3_e);
  assert_order_is(-9_e / 10_e, 9_e / 10_e);
  assert_order_is(2_e / 3_e, 3_e / 4_e);
  assert_order_is(KOpposite(KDecimal(1_e, 2_e)), KOpposite(KDecimal(1_e, 3_e)));
  assert_order_is(KDecimal(12_e, 3_e), KDecimal(123_e, 3_e));
  assert_order_is(KDecimal(1_e, 3_e),
                  KOpposite(KDecimal(1_e, 3_e)));  // Should be reversed?
  assert_order_is(KDecimal(1_e, 3_e),
                  KDecimal(1_e, 4_e));  // Should be reversed?

  // Powers
  assert_order_is(KPow(1_e / 2_e, 5_e), KPow(2_e, 3_e));
  assert_order_is("2^2", "2^3");
  assert_order_is("1", "2^3");
  assert_order_is("2", "2^3");

  // Constants
  assert_order_is(π_e, e_e);
  assert_order_is(π_e, i_e);
  assert_order_is(e_e, i_e);
  assert_order_is("e", "_c");
  assert_order_is("pi", "G");
  assert_order_is(π_e, KAdd(2_e, π_e));

  // Variables
  assert_order_is("a"_e, "a"_e, 0);
  assert_order_is("a"_e, "b"_e);
  assert_order_is("b"_e, "a"_e, 1);
  assert_order_is(KAdd(2_e, "a"_e), KAdd(3_e, "a"_e));
  assert_order_is(KPow("x"_e, -1_e), "x"_e);
  assert_order_is("x", "y");
  assert_order_is("x", "x^2");
  assert_order_is("w^2", "x");
  assert_order_is("x", "y^2");
  assert_order_is("x", "1+y");
  assert_order_is("1+y", "z");
  assert_order_is("y", "x*z");

  // Units
  assert_order_is("_yd", "_in", -1, Order::OrderType::Beautification);
  assert_order_is("_yd", "_au", 1, Order::OrderType::Beautification);

  // Functions
  assert_order_is(KFun<"f">(9_e), KFun<"g">(1_e));
  assert_order_is("a(x)", "b(y)");
  assert_order_is("a(1)", "a(2)");
  assert_order_is("gcd(0, 1, 4)", "gcd(0, 2, 3)");
  assert_order_is("2 + 3", "1 + 4");
  assert_order_is(KTrig(5_e, 0_e), KTrig("x"_e, 4_e));

  // Others
  assert_order_is("x"_e, KInf);
  assert_order_is(2_e, "x"_e);
}

void assert_sorts_to(const char* input, const char* output,
                     Order::OrderType order) {
  static Order::OrderType s_order;
  s_order = order;
  process_tree_and_compare(
      input, output,
      [](Tree* e, Poincare::ProjectionContext ctx) { NAry::Sort(e, s_order); },
      {});
}

QUIZ_CASE(pcj_sort) {
  // Beautification
  assert_sorts_to("2*5", "2*5", Beautification);
  assert_sorts_to("5*2", "2*5", Beautification);
  assert_sorts_to("√(3)*2", "2*√(3)", Beautification);
  assert_sorts_to("π*2*i", "2*π*i", Beautification);
  // TODO_PCJ: √(2) should be before π and unknowns
  assert_sorts_to("π*2*√(2)*_m", "2×π×√(2)×_m", Beautification);
  assert_sorts_to("π*2*√(2)*i*a*u", "2×π×a×u×√(2)×i", Beautification);
  assert_sorts_to("_in+3_yd+3_ft", "3_yd+3_ft+_in", Beautification);
  assert_sorts_to("π^b*π^a", "π^a*π^b", Beautification);
  assert_sorts_to("π^3*π^2", "π^2*π^3", Beautification);
  assert_sorts_to("1+[[3]]+2", "1+2+[[3]]", Beautification);
  assert_sorts_to("1+[[3]]+[[0]]+2", "1+2+[[0]]+[[3]]", Beautification);
  assert_sorts_to("i+sum([[0]],n,0,0)", "sum([[0]],n,0,0)+i", Beautification);

  // AdditionBeautification
  assert_sorts_to("1+2+0", "0+1+2", AdditionBeautification);
  assert_sorts_to("π+i+e", "π+e+i", AdditionBeautification);
  assert_sorts_to("π+2+i", "2+π+i", AdditionBeautification);
  assert_sorts_to("b+b^2+a^2+a^3", "a^3+a^2+b^2+b", AdditionBeautification);
  assert_sorts_to("c+b^2+a^2+a", "a^2+b^2+a+c", AdditionBeautification);
  assert_sorts_to("3*x^2+2*x^3", "2*x^3+3*x^2", AdditionBeautification);
  assert_sorts_to("2*x+3*x", "2*x+3*x", AdditionBeautification);

  Tree* e = KAdd(KVarX, KPow(KVarX, 2_e), KPow(KVarX, 1_e / 2_e))->cloneTree();
  NAry::Sort(e, AdditionBeautification);
  assert_trees_are_equal(e,
                         KAdd(KPow(KVarX, 2_e), KVarX, KPow(KVarX, 1_e / 2_e)));
}

void assert_contain_subtree(const Tree* tree, const Tree* subtree) {
  quiz_assert(Order::ContainsSubtree(tree, subtree));
}

void assert_not_contain_subtree(const Tree* tree, const Tree* subtree) {
  quiz_assert(!Order::ContainsSubtree(tree, subtree));
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
}

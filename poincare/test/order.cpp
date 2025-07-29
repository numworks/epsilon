#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/order.h>
#include <poincare/src/memory/n_ary.h>

#include "helper.h"

using namespace Poincare::Internal;
using enum Order::OrderType;

void assert_order_is(const Tree* e1, const Tree* e2, int result,
                     Order::OrderType order = System) {
  int comparison = Order::Compare(e1, e2, order);
  quiz_assert(comparison == result);
}

void assert_order_is(const char* t1, const char* t2, int result,
                     Order::OrderType order = System) {
  Tree* e1 = parse(t1);
  Tree* e2 = parse(t2);
  assert_order_is(e1, e2, result, order);
  e2->removeTree();
  e1->removeTree();
}

QUIZ_CASE(pcj_order) {
  // Numbers
  assert_order_is(10.0_fe, 2.0_fe, 1);
  assert_order_is(1.0_fe, 2.0_fe, -1);
  assert_order_is(0.234_de, -0.2392_de, 1);
  assert_order_is(0.234_fe, 0.123_fe, 1);
  assert_order_is(234_de, 123_de, 1);
  assert_order_is(2_e, 3_e, -1);
  assert_order_is(9_e / 10_e, -9_e / 10_e, 1);
  assert_order_is(3_e / 4_e, 2_e / 3_e, 1);
  assert_order_is(KOpposite(KDecimal(1_e, 3_e)), KOpposite(KDecimal(1_e, 2_e)),
                  1);
  assert_order_is(KDecimal(123_e, 3_e), KDecimal(12_e, 3_e), 1);
  assert_order_is(KDecimal(1_e, 3_e), KOpposite(KDecimal(1_e, 3_e)),
                  -1);                                          // Should be 1?
  assert_order_is(KDecimal(1_e, 3_e), KDecimal(1_e, 4_e), -1);  // Should be 1?

  // Powers
  assert_order_is(KPow(1_e / 2_e, 5_e), KPow(2_e, 3_e), -1);
  assert_order_is("2^2", "2^3", -1);
  assert_order_is("1", "2^3", -1);
  assert_order_is("2", "2^3", -1);

  // Constants
  assert_order_is(π_e, e_e, -1);
  assert_order_is(π_e, i_e, -1);
  assert_order_is(e_e, i_e, -1);
  assert_order_is("e", "_c", -1);
  assert_order_is("pi", "G", -1);
  assert_order_is(KAdd(2_e, π_e), π_e, 1);

  // Variables
  assert_order_is("a"_e, "a"_e, 0);
  assert_order_is("a"_e, "b"_e, -1);
  assert_order_is("b"_e, "a"_e, 1);
  assert_order_is(KAdd(3_e, "a"_e), KAdd(2_e, "a"_e), 1);
  assert_order_is("x"_e, KPow("x"_e, -1_e), 1);
  assert_order_is("y", "x", 1);
  assert_order_is("x^2", "x", 1);
  assert_order_is("x", "w^2", 1);
  assert_order_is("y^2", "x", 1);
  assert_order_is("1+y", "x", 1);
  assert_order_is("z", "1+y", 1);
  assert_order_is("x*z", "y", 1);

  // Units
  assert_order_is("_yd", "_in", -1, Order::OrderType::Beautification);
  assert_order_is("_yd", "_au", 1, Order::OrderType::Beautification);

  // Functions
  assert_order_is(KFun<"g">(1_e), KFun<"f">(9_e), 1);
  assert_order_is("b(y)", "a(x)", 1);
  assert_order_is("a(2)", "a(1)", 1);
  assert_order_is("gcd(0, 2, 3)", "gcd(0, 1, 4)", 1);
  assert_order_is("1 + 4", "2 + 3", 1);
  assert_order_is(KTrig("x"_e, 4_e), KTrig(5_e, 0_e), 1);

  // Others
  assert_order_is(KInf, "x"_e, 1);
  assert_order_is("x"_e, 2_e, 1);
}

void assert_sorts_to(const char* input, const char* output,
                     Order::OrderType order) {
  static Order::OrderType s_order;
  s_order = order;
  process_tree_and_compare(
      input, output,
      [](Tree* e, ProjectionContext ctx) { NAry::Sort(e, s_order); }, {});
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

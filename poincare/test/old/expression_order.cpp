#include <poincare/print.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/order.h>
#include <poincare/src/memory/n_ary.h>

#include "../helper.h"

using namespace Poincare::Internal;

void assert_is_greater(const Tree* e1, const Tree* e2) {
  quiz_assert(Order::CompareSystem(e1, e2) == 1);
}

void assert_greater(const Tree* e1, const Tree* e2) {
  bool test = Order::CompareSystem(e1, e2) == 1;

  constexpr int bufferSize = 256;
  char information[bufferSize] = "";
  char buffer1[bufferSize];
  char buffer2[bufferSize];
  serialize_expression(e1, buffer1, bufferSize);
  serialize_expression(e2, buffer2, bufferSize);
  Poincare::Print::UnsafeCustomPrintf(information, bufferSize,
                                      "%s\t%s greater than %s",
                                      !test ? "BAD" : "OK", buffer1, buffer2);
  quiz_print(information);
}

QUIZ_CASE(poincare_expression_order_constant) {
  Tree* c = parse("_c");
  assert_greater(c, e_e);
  c->removeTree();
  Tree* G = parse("_G");
  assert_greater(G, π_e);
  G->removeTree();
  assert_greater(π_e, i_e);
  assert_greater(e_e, π_e);
  assert_greater(e_e, i_e);
}

QUIZ_CASE(poincare_expression_order_decimal) {
  assert_greater(KDecimal(1_e, 3_e), KOpposite(KDecimal(1_e, 3_e)));
  assert_greater(KOpposite(KDecimal(1_e, 3_e)), KOpposite(KDecimal(1_e, 2_e)));
  assert_greater(KDecimal(1_e, 3_e), KDecimal(1_e, 4_e));
  assert_greater(KDecimal(123_e, 3_e), KDecimal(12_e, 3_e));
}

QUIZ_CASE(poincare_expression_order_rational) {
  assert_is_greater(9_e / 10_e, -9_e / 10_e);
  assert_is_greater(3_e / 4_e, 2_e / 3_e);
}

QUIZ_CASE(poincare_expression_order_float) {
  assert_is_greater(0.234_de, -0.2392_de);
  assert_is_greater(0.234_fe, 0.123_fe);
  assert_is_greater(234_de, 123_de);
}

QUIZ_CASE(poincare_expression_order_power) {
  // 2^3 > (1/2)^5
  assert_is_greater(KPow(2_e, 3_e), KPow(1_e / 2_e, 5_e));
  // 2^3 > 2^2
  assert_is_greater(KPow(2_e, 3_e), KPow(2_e, 2_e));
  // Order with expression other than power
  // 2^3 > 1
  assert_is_greater(KPow(2_e, 3_e), 1_e);
  // 2^3 > 2
  assert_is_greater(KPow(2_e, 3_e), 2_e);
}

QUIZ_CASE(poincare_expression_order_symbol) { assert_greater("a"_e, "b"_e); }

QUIZ_CASE(poincare_expression_order_function) {
  assert_greater(KFun<"f">(1_e), KFun<"g">(9_e));
}

QUIZ_CASE(poincare_expression_order_mix) {
  assert_greater("x"_e, 2_e);
  assert_greater("x"_e, KInf);
  assert_greater(KACos(2_e), KDecimal(3_e, 2_e));
}

void assert_multiplication_or_addition_is_ordered_as(const Tree* e1,
                                                     const Tree* e2) {
  Tree* clone = e1->cloneTree();
  NAry::Sort(clone, e1->isAdd() ? Order::OrderType::AdditionBeautification
                                : Order::OrderType::Beautification);
  bool test = e1->treeIsIdenticalTo(e2);
  // quiz_assert(e1->treeIsIdenticalTo(e2));

  constexpr int bufferSize = 256;
  char information[bufferSize] = "";
  char buffer1[bufferSize];
  char buffer2[bufferSize];
  serialize_expression(e1, buffer1, bufferSize);
  serialize_expression(e2, buffer2, bufferSize);
  int i = Poincare::Print::UnsafeCustomPrintf(
      information, bufferSize, "%s\t%s\t%s", !test ? "BAD" : "OK", buffer1,
      buffer2);
  if (!test) {
    serialize_expression(clone, buffer2, bufferSize);
    Poincare::Print::UnsafeCustomPrintf(information + i, bufferSize - i, "\t%s",
                                        buffer2);
  }
  quiz_print(information);

  clone->removeTree();
}

QUIZ_CASE(poincare_expression_order_addition_multiplication) {
  {
    // 2 * 5 -> 2 * 5
    assert_multiplication_or_addition_is_ordered_as(KMult(2_e, 5_e),
                                                    KMult(2_e, 5_e));
  }
  {
    // 5 * 2 -> 2 * 5
    assert_multiplication_or_addition_is_ordered_as(KMult(5_e, 2_e),
                                                    KMult(2_e, 5_e));
  }
  {
    // 1 + 2 + 0 -> 2 + 1 + 0
    assert_multiplication_or_addition_is_ordered_as(KAdd(1_e, 2_e, 0_e),
                                                    KAdd(2_e, 1_e, 0_e));
  }
  {
    // pi + i + e -> e + pi + i
    assert_multiplication_or_addition_is_ordered_as(KAdd(π_e, i_e, e_e),
                                                    KAdd(e_e, π_e, i_e));
  }
  {
    // root(3) * 2 -> 2 * root(3)
    assert_multiplication_or_addition_is_ordered_as(KMult(KSqrt(3_e), 2_e),
                                                    KMult(2_e, KSqrt(3_e)));
  }
  {
    // c + b^2 + a^2 + a -> a^2 + a + b^2 + c
    assert_multiplication_or_addition_is_ordered_as(
        KAdd("c"_e, KPow("b"_e, 2_e), KPow("a"_e, 2_e), "a"_e),
        KAdd(KPow("a"_e, 2_e), "a"_e, KPow("b"_e, 2_e), "c"_e));
  }
  {
    // 3*x^2 + 2*x^3 -> 2*x^3 + 3*x^2
    assert_multiplication_or_addition_is_ordered_as(
        KAdd(KMult(3_e, KPow("x"_e, 2_e)), KMult(2_e, KPow("x"_e, 3_e))),
        KAdd(KMult(2_e, KPow("x"_e, 3_e)), KMult(3_e, KPow("x"_e, 2_e))));
  }
  {
    // 2*x + 3*x -> 3*x + 2*x
    assert_multiplication_or_addition_is_ordered_as(
        KAdd(KMult(2_e, "x"_e), KMult(3_e, "x"_e)),
        KAdd(KMult(3_e, "x"_e), KMult(2_e, "x"_e)));
  }
  {
    // pi^b * pi^a -> pi^a * pi^b
    assert_multiplication_or_addition_is_ordered_as(
        KMult(KPow(π_e, "b"_e), KPow(π_e, "a"_e)),
        KMult(KPow(π_e, "a"_e), KPow(π_e, "b"_e)));
  }
  {
    // pi^3 * pi^2 -> pi^2 * pi^3
    assert_multiplication_or_addition_is_ordered_as(
        KMult(KPow(π_e, 3_e), KPow(π_e, 2_e)),
        KMult(KPow(π_e, 2_e), KPow(π_e, 3_e)));
  }
  {
    // 1 + Matrix1 + 2 -> 1 + 2 + Matrix1
    assert_multiplication_or_addition_is_ordered_as(
        KMult(2_e, KMatrix<1, 1>(3_e), 1_e),
        KMult(1_e, 2_e, KMatrix<1, 1>(3_e)));
  }
  {
    // 1 + Matrix1 + Matrix2 + 2 -> 1 + 2 + Matrix1 + Matrix2
    assert_multiplication_or_addition_is_ordered_as(
        KMult(2_e, KMatrix<1, 1>(3_e), KMatrix<1, 1>(0_e), 1_e),
        KMult(1_e, 2_e, KMatrix<1, 1>(3_e), KMatrix<1, 1>(0_e)));
  }
  {
    // ∑Matrix + i  -> i + ∑Matrix
    assert_multiplication_or_addition_is_ordered_as(
        KAdd(KSum(KMatrix<1, 1>(0_e), "n"_e, 0_e, 0_e), "i"_e),
        KAdd("i"_e, KSum(KMatrix<1, 1>(0_e), "n"_e, 0_e, 0_e)));
  }
}

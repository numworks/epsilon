#include <apps/shared/global_context.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/constant.h>
#include <poincare/old/infinity.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/square_root.h>
#include <poincare/old/sum.h>

#include "helper.h"

using namespace Poincare;

void assert_greater(OExpression e1, OExpression e2) {
  /* ExpressionNode::SimplificationOrder can be used directly (because node
   * getter is private) so we build an addition whose we sort children and we
   * check that the children order is the expected one. */
  Shared::GlobalContext globalContext;
  Addition a = Addition::Builder(e1, e2);
  a.sortChildrenInPlace(
      [](const ExpressionNode *e1, const ExpressionNode *e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      &globalContext, true);
  quiz_assert(a.childAtIndex(0) == e1);
  quiz_assert(a.childAtIndex(1) == e2);
}

QUIZ_CASE(poincare_expression_order_constant) {
  assert_greater(Constant::Builder("_c"), Constant::ExponentialEBuilder());
  assert_greater(Constant::Builder("_G"), Constant::PiBuilder());
  assert_greater(Constant::PiBuilder(), Constant::ComplexIBuilder());
  assert_greater(Constant::ExponentialEBuilder(), Constant::PiBuilder());
  assert_greater(Constant::ExponentialEBuilder(), Constant::ComplexIBuilder());
}

QUIZ_CASE(poincare_expression_order_decimal) {
  assert_greater(Decimal::Builder("1", -3), Decimal::Builder("-1", -3));
  assert_greater(Decimal::Builder("-1", -3), Decimal::Builder("-1", -2));
  assert_greater(Decimal::Builder("1", -3), Decimal::Builder("1", -4));
  assert_greater(Decimal::Builder("123", -3), Decimal::Builder("12", -3));
}

QUIZ_CASE(poincare_expression_order_rational) {
  assert_greater(Rational::Builder(9, 10), Rational::Builder(-9, 10));
  assert_greater(Rational::Builder(3, 4), Rational::Builder(2, 3));
}

QUIZ_CASE(poincare_expression_order_float) {
  assert_greater(Float<double>::Builder(0.234),
                 Float<double>::Builder(-0.2392));
  assert_greater(Float<float>::Builder(0.234), Float<float>::Builder(0.123));
  assert_greater(Float<double>::Builder(234), Float<double>::Builder(123));
}

QUIZ_CASE(poincare_expression_order_power) {
  // 2^3 > (1/2)^5
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)),
                 Power::Builder(Rational::Builder(1, 2), Rational::Builder(5)));
  // 2^3 > 2^2
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)),
                 Power::Builder(Rational::Builder(2), Rational::Builder(2)));
  // Order with expression other than power
  // 2^3 > 1
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)),
                 Rational::Builder(1));
  // 2^3 > 2
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)),
                 Rational::Builder(2));
}

QUIZ_CASE(poincare_expression_order_symbol) {
  assert_greater(Symbol::Builder('a'), Symbol::Builder('b'));
}

QUIZ_CASE(poincare_expression_order_function) {
  assert_greater(Function::Builder("f", 1, Rational::Builder(1)),
                 Function::Builder("g", 1, Rational::Builder(9)));
}

QUIZ_CASE(poincare_expression_order_mix) {
  assert_greater(Symbol::Builder('x'), Rational::Builder(2));
  assert_greater(Symbol::Builder('x'), Infinity::Builder(true));
  assert_greater(ArcCosine::Builder(Rational::Builder(2)),
                 Decimal::Builder("3", -2));
}

void assert_multiplication_or_addition_is_ordered_as(OExpression e1,
                                                     OExpression e2) {
  Shared::GlobalContext globalContext;
  if (e1.otype() == ExpressionNode::Type::Multiplication) {
    static_cast<Multiplication &>(e1).sortChildrenInPlace(
        [](const ExpressionNode *e1, const ExpressionNode *e2) {
          return ExpressionNode::SimplificationOrder(e1, e2, true);
        },
        &globalContext);
  } else {
    quiz_assert(e1.otype() == ExpressionNode::Type::Addition);
    static_cast<Addition &>(e1).sortChildrenInPlace(
        [](const ExpressionNode *e1, const ExpressionNode *e2) {
          return ExpressionNode::SimplificationOrder(e1, e2, false);
        },
        &globalContext);
  }
  quiz_assert(e1.isIdenticalTo(e2));
}

QUIZ_CASE(poincare_expression_order_addition_multiplication) {
  {
    // 2 * 5 -> 2 * 5
    OExpression e1 =
        Multiplication::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e1);
  }
  {
    // 5 * 2 -> 2 * 5
    OExpression e1 =
        Multiplication::Builder(Rational::Builder(5), Rational::Builder(2));
    OExpression e2 =
        Multiplication::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + 2 + 0 -> 2 + 1 + 0
    OExpression e1 = Addition::Builder(
        {Rational::Builder(1), Rational::Builder(2), Rational::Builder(0)});
    OExpression e2 = Addition::Builder(
        {Rational::Builder(2), Rational::Builder(1), Rational::Builder(0)});
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi + i + e -> e + pi + i
    OExpression pi = Constant::PiBuilder();
    OExpression i = Constant::ComplexIBuilder();
    OExpression e = Constant::ExponentialEBuilder();
    OExpression e1 = Addition::Builder({pi.clone(), i.clone(), e.clone()});
    OExpression e2 = Addition::Builder({e, pi, i});
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // root(3) * 2 -> 2 * root(3)
    OExpression e1 = Multiplication::Builder(
        SquareRoot::Builder(Rational::Builder(3)), Rational::Builder(2));
    OExpression e2 = Multiplication::Builder(
        Rational::Builder(2), SquareRoot::Builder(Rational::Builder(3)));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // c + b^2 + a^2 + a -> a^2 + a + b^2 + c
    OExpression e1 = Addition::Builder(
        {Symbol::Builder('c'),
         Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
         Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
         Symbol::Builder('a')});
    OExpression e2 = Addition::Builder(
        {Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
         Symbol::Builder('a'),
         Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
         Symbol::Builder('c')});
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 3*x^2 + 2*x^3 -> 2*x^3 + 3*x^2
    OExpression child1 = Multiplication::Builder(
        Rational::Builder(2),
        Power::Builder(Symbol::Builder('x'), Rational::Builder(3)));
    OExpression child2 = Multiplication::Builder(
        Rational::Builder(3),
        Power::Builder(Symbol::Builder('x'), Rational::Builder(2)));
    OExpression e1 = Addition::Builder(child2.clone(), child1.clone());
    OExpression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2*x + 3*x -> 3*x + 2*x
    OExpression child1 =
        Multiplication::Builder(Rational::Builder(3), Symbol::Builder('x'));
    OExpression child2 =
        Multiplication::Builder(Rational::Builder(2), Symbol::Builder('x'));
    OExpression e1 = Addition::Builder(child2.clone(), child1.clone());
    OExpression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^b * pi^a -> pi^a * pi^b
    OExpression child1 =
        Power::Builder(Constant::PiBuilder(), Symbol::Builder('a'));
    OExpression child2 =
        Power::Builder(Constant::PiBuilder(), Symbol::Builder('b'));
    OExpression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    OExpression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^3 * pi^2 -> pi^2 * pi^3
    OExpression child1 =
        Power::Builder(Constant::PiBuilder(), Rational::Builder(2));
    OExpression child2 =
        Power::Builder(Constant::PiBuilder(), Rational::Builder(3));
    OExpression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    OExpression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + Matrix1 + 2 -> 1 + 2 + Matrix1
    OExpression child1 = Rational::Builder(1);
    OExpression child2 = Rational::Builder(2);
    OExpression childMatrix = OMatrix::Builder();
    static_cast<OMatrix &>(childMatrix)
        .addChildAtIndexInPlace(Rational::Builder(3), 0, 0);
    OExpression e1 = Multiplication::Builder(
        child2.clone(), childMatrix.clone(), child1.clone());
    OExpression e2 = Multiplication::Builder(child1.clone(), child2.clone(),
                                             childMatrix.clone());
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

  {
    // 1 + Matrix1 + Matrix2 + 2 -> 1 + 2 + Matrix1 + Matrix2
    OExpression child1 = Rational::Builder(1);
    OExpression child2 = Rational::Builder(2);
    OExpression childMatrix1 = OMatrix::Builder();
    static_cast<OMatrix &>(childMatrix1)
        .addChildAtIndexInPlace(Rational::Builder(3), 0, 0);
    OExpression childMatrix2 = OMatrix::Builder();
    static_cast<OMatrix &>(childMatrix2)
        .addChildAtIndexInPlace(Rational::Builder(0), 0, 0);

    OExpression e1 =
        Multiplication::Builder({child2.clone(), childMatrix1.clone(),
                                 childMatrix2.clone(), child1.clone()});
    OExpression e2 =
        Multiplication::Builder({child1.clone(), child2.clone(),
                                 childMatrix1.clone(), childMatrix2.clone()});
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

  {
    // ∑OMatrix + i  -> i + ∑OMatrix
    OExpression childMatrix = OMatrix::Builder();
    static_cast<OMatrix &>(childMatrix)
        .addChildAtIndexInPlace(Rational::Builder(0), 0, 0);
    OExpression child1 =
        Sum::Builder(childMatrix, Symbol::Builder('n'), Rational::Builder(0),
                     Rational::Builder(0));
    OExpression child2 = Symbol::Builder('i');
    OExpression e1 = Addition::Builder(child1.clone(), child2.clone());
    OExpression e2 = Addition::Builder(child2, child1);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
}

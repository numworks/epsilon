#include <apps/shared/global_context.h>
#include <poincare/addition.h>
#include <poincare/arc_cosine.h>
#include <poincare/constant.h>
#include <poincare/infinity.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/square_root.h>
#include <poincare/sum.h>
#include "helper.h"

using namespace Poincare;

void assert_greater(Expression e1, Expression e2) {
  /* ExpressionNode::SimplificationOrder can be used directly (because node
   * getter is private) so we build an addition whose we sort children and we
   * check that the children order is the expected one. */
  Shared::GlobalContext globalContext;
  Addition a = Addition::Builder(e1, e2);
  a.sortChildrenInPlace(
      [](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, false); },
      &globalContext, true);
  quiz_assert(a.childAtIndex(0) == e1);
  quiz_assert(a.childAtIndex(1) == e2);
}

QUIZ_CASE(poincare_expression_order_constant) {
  assert_greater(Constant::Builder("_c"), Constant::Builder("e"));
  assert_greater(Constant::Builder("_G"), Constant::Builder("π"));
  assert_greater(Constant::Builder("π"), Constant::Builder("i"));
  assert_greater(Constant::Builder("e"), Constant::Builder("π"));
  assert_greater(Constant::Builder("e"), Constant::Builder("i"));
}

QUIZ_CASE(poincare_expression_order_decimal) {
  assert_greater(Decimal::Builder("1", -3), Decimal::Builder("-1", -3));
  assert_greater(Decimal::Builder("-1", -3), Decimal::Builder("-1", -2));
  assert_greater(Decimal::Builder("1", -3), Decimal::Builder("1", -4));
  assert_greater(Decimal::Builder("123", -3), Decimal::Builder("12", -3));
}

QUIZ_CASE(poincare_expression_order_rational) {
  assert_greater(Rational::Builder(9, 10), Rational::Builder(-9,10));
  assert_greater(Rational::Builder(3, 4), Rational::Builder(2,3));
}

QUIZ_CASE(poincare_expression_order_float) {
  assert_greater(Float<double>::Builder(0.234), Float<double>::Builder(-0.2392));
  assert_greater(Float<float>::Builder(0.234), Float<float>::Builder(0.123));
  assert_greater(Float<double>::Builder(234), Float<double>::Builder(123));
}

QUIZ_CASE(poincare_expression_order_power) {
  // 2^3 > (1/2)^5
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)), Power::Builder(Rational::Builder(1,2), Rational::Builder(5)));
  // 2^3 > 2^2
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)), Power::Builder(Rational::Builder(2), Rational::Builder(2)));
  // Order with expression other than power
  // 2^3 > 1
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)), Rational::Builder(1));
  // 2^3 > 2
  assert_greater(Power::Builder(Rational::Builder(2), Rational::Builder(3)), Rational::Builder(2));
}

QUIZ_CASE(poincare_expression_order_symbol) {
  assert_greater(Symbol::Builder('a'), Symbol::Builder('b'));
}

QUIZ_CASE(poincare_expression_order_function) {
  assert_greater(Function::Builder("f", 1, Rational::Builder(1)), Function::Builder("g", 1, Rational::Builder(9)));
}

QUIZ_CASE(poincare_expression_order_mix) {
  assert_greater(Symbol::Builder('x'), Rational::Builder(2));
  assert_greater(Symbol::Builder('x'), Infinity::Builder(true));
  assert_greater(ArcCosine::Builder(Rational::Builder(2)), Decimal::Builder("3",-2));
}

void assert_multiplication_or_addition_is_ordered_as(Expression e1, Expression e2) {
  Shared::GlobalContext globalContext;
  if (e1.type() == ExpressionNode::Type::Multiplication) {
    static_cast<Multiplication&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, true); },
        &globalContext);
  } else {
    quiz_assert(e1.type() == ExpressionNode::Type::Addition);
    static_cast<Addition&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, false); },
        &globalContext);
  }
  quiz_assert(e1.isIdenticalTo(e2));
}

QUIZ_CASE(poincare_expression_order_addition_multiplication) {
  {
    // 2 * 5 -> 2 * 5
    Expression e1 = Multiplication::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e1);
  }
  {
    // 5 * 2 -> 2 * 5
    Expression e1 = Multiplication::Builder(Rational::Builder(5), Rational::Builder(2));
    Expression e2 = Multiplication::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + 2 + 0 -> 2 + 1 + 0
    Expression e1 = Addition::Builder({
      Rational::Builder(1),
      Rational::Builder(2),
      Rational::Builder(0)
    });
    Expression e2 = Addition::Builder({
      Rational::Builder(2),
      Rational::Builder(1),
      Rational::Builder(0)
    });
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi + i + e -> e + pi + i
    Expression pi = Constant::Builder("π");
    Expression i = Constant::Builder("i");
    Expression e = Constant::Builder("e");
    Expression e1 = Addition::Builder({pi.clone(), i.clone(), e.clone()});
    Expression e2 = Addition::Builder({e, pi, i});
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // root(3) * 2 -> 2 * root(3)
    Expression e1 = Multiplication::Builder(SquareRoot::Builder(Rational::Builder(3)), Rational::Builder(2));
    Expression e2 = Multiplication::Builder(Rational::Builder(2), SquareRoot::Builder(Rational::Builder(3)));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // c + b^2 + a^2 + a -> a^2 + a + b^2 + c
    Expression e1 = Addition::Builder({
      Symbol::Builder('c'),
      Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
      Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
      Symbol::Builder('a')
    });
    Expression e2 = Addition::Builder({
      Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
      Symbol::Builder('a'),
      Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
      Symbol::Builder('c')
    });
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 3*x^2 + 2*x^3 -> 2*x^3 + 3*x^2
    Expression child1 = Multiplication::Builder(Rational::Builder(2), Power::Builder(Symbol::Builder('x'), Rational::Builder(3)));
    Expression child2 = Multiplication::Builder(Rational::Builder(3), Power::Builder(Symbol::Builder('x'), Rational::Builder(2)));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2*x + 3*x -> 3*x + 2*x
    Expression child1 = Multiplication::Builder(Rational::Builder(3), Symbol::Builder('x'));
    Expression child2 = Multiplication::Builder(Rational::Builder(2), Symbol::Builder('x'));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^b * pi^a -> pi^a * pi^b
    Expression child1 = Power::Builder(Constant::Builder("π"), Symbol::Builder('a'));
    Expression child2 = Power::Builder(Constant::Builder("π"), Symbol::Builder('b'));
    Expression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    Expression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^3 * pi^2 -> pi^2 * pi^3
    Expression child1 = Power::Builder(Constant::Builder("π"), Rational::Builder(2));
    Expression child2 = Power::Builder(Constant::Builder("π"), Rational::Builder(3));
    Expression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    Expression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + Matrix1 + 2 -> 1 + 2 + Matrix1
    Expression child1 = Rational::Builder(1);
    Expression child2 = Rational::Builder(2);
    Expression childMatrix = Matrix::Builder();
    static_cast<Matrix &>(childMatrix).addChildAtIndexInPlace(Rational::Builder(3), 0, 0);
    Expression e1 = Multiplication::Builder(child2.clone(), childMatrix.clone(), child1.clone());
    Expression e2 = Multiplication::Builder(child1.clone(), child2.clone(), childMatrix.clone());
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

  {
    // 1 + Matrix1 + Matrix2 + 2 -> 1 + 2 + Matrix1 + Matrix2
    Expression child1 = Rational::Builder(1);
    Expression child2 = Rational::Builder(2);
    Expression childMatrix1 = Matrix::Builder();
    static_cast<Matrix &>(childMatrix1).addChildAtIndexInPlace(Rational::Builder(3), 0, 0);
    Expression childMatrix2 = Matrix::Builder();
    static_cast<Matrix &>(childMatrix2).addChildAtIndexInPlace(Rational::Builder(0), 0, 0);

    Expression e1 = Multiplication::Builder({
      child2.clone(),
      childMatrix1.clone(),
      childMatrix2.clone(),
      child1.clone()
    });
    Expression e2 = Multiplication::Builder({
      child1.clone(),
      child2.clone(),
      childMatrix1.clone(),
      childMatrix2.clone()
    });
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

  {
    // ∑Matrix + i  -> i + ∑Matrix
    Expression childMatrix = Matrix::Builder();
    static_cast<Matrix &>(childMatrix).addChildAtIndexInPlace(Rational::Builder(0), 0, 0);
    Expression child1 = Sum::Builder(childMatrix, Symbol::Builder('n'), Rational::Builder(0), Rational::Builder(0));
    Expression child2 = Symbol::Builder('i');
    Expression e1 = Addition::Builder(child1.clone(), child2.clone());
    Expression e2 = Addition::Builder(child2, child1);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
}

#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <poincare/include/poincare_nodes.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

// TODO add tests about expression that override simplificationOrderSameType or simplificationOrderGreaterType

static inline void assert_equal(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) == 0);
}
static inline void assert_not_equal(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) != 0);
}

static inline void assert_lower(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) < 0);
}

static inline void assert_greater(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) > 0);
}

QUIZ_CASE(poincare_expression_order_rational) {
  assert_equal(Rational::Builder(123,324), Rational::Builder(41,108));
  assert_not_equal(Rational::Builder(123,234), Rational::Builder(42, 108));
  assert_lower(Rational::Builder(123,234), Rational::Builder(456,567));
  assert_lower(Rational::Builder(-123, 234),Rational::Builder(456, 567));
  assert_greater(Rational::Builder(123, 234),Rational::Builder(-456, 567));
  assert_greater(Rational::Builder(123, 234),Rational::Builder("123456789123456789", "12345678912345678910"));
}

void assert_multiplication_or_addition_is_ordered_as(Expression e1, Expression e2) {
  Shared::GlobalContext globalContext;
  if (e1.type() == ExpressionNode::Type::MultiplicationExplicite) {
    static_cast<MultiplicationExplicite&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); },
        &globalContext,
        true);
  } else {
    assert(e1.type() == ExpressionNode::Type::Addition);
    static_cast<Addition&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); },
        &globalContext,
        true);
  }
  quiz_assert(e1.isIdenticalTo(e2));
}

QUIZ_CASE(poincare_expression_order_addition_multiplication) {
  {
    // 2 * 5 -> 2 * 5
    Expression e1 = MultiplicationExplicite::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e1);
  }
  {
    // 5 * 2 -> 2 * 5
    Expression e1 = MultiplicationExplicite::Builder(Rational::Builder(5), Rational::Builder(2));
    Expression e2 = MultiplicationExplicite::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + 2 + 0 -> 2 + 1 + 0
    constexpr int numberOfChildren = 3;
    Expression children[numberOfChildren] = {Rational::Builder(1), Rational::Builder(2), Rational::Builder(0)};
    Expression childrenSorted[numberOfChildren] = {Rational::Builder(2), Rational::Builder(1), Rational::Builder(0)};
    Expression e1 = Addition::Builder(children, numberOfChildren);
    Expression e2 = Addition::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi + i + e -> e + pi + i
    Expression pi = Constant::Builder(UCodePointGreekSmallLetterPi);
    Expression i = Constant::Builder(UCodePointMathematicalBoldSmallI);
    Expression e = Constant::Builder(UCodePointScriptSmallE);
    constexpr int numberOfChildren = 3;
    Expression children[numberOfChildren] = {pi.clone(), i.clone(), e.clone()};
    Expression childrenSorted[numberOfChildren] = {e, pi, i};
    Expression e1 = Addition::Builder(children, numberOfChildren);
    Expression e2 = Addition::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // root(3) $ 2 -> 2 * root(3)
    Expression e1 = MultiplicationExplicite::Builder(SquareRoot::Builder(Rational::Builder(3)), Rational::Builder(2));
    Expression e2 = MultiplicationExplicite::Builder(Rational::Builder(2), SquareRoot::Builder(Rational::Builder(3)));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    constexpr int numberOfChildren = 4;
    Expression children[numberOfChildren] = {
      Symbol::Builder('c'),
      Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
      Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
      Symbol::Builder('a')
    };
    Expression childrenSorted[numberOfChildren] = {
      Power::Builder(Symbol::Builder('a'), Rational::Builder(2)),
      Symbol::Builder('a'),
      Power::Builder(Symbol::Builder('b'), Rational::Builder(2)),
      Symbol::Builder('c')
    };
    // c + b^2 + a^2 + a -> a^2 + a + b^2 + c
    Expression e1 = Addition::Builder(children, numberOfChildren);
    Expression e2 = Addition::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 3*x^2 + 2*x^3 -> 2*x^3 + 3*x^2
    Expression child1 = MultiplicationExplicite::Builder(Rational::Builder(2), Power::Builder(Symbol::Builder('x'), Rational::Builder(3)));
    Expression child2 = MultiplicationExplicite::Builder(Rational::Builder(3), Power::Builder(Symbol::Builder('x'), Rational::Builder(2)));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2*x + 3*x -> 3*x + 2*x
    Expression child1 = MultiplicationExplicite::Builder(Rational::Builder(3), Symbol::Builder('x'));
    Expression child2 = MultiplicationExplicite::Builder(Rational::Builder(2), Symbol::Builder('x'));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^b * pi^a -> pi^a * pi^b
    Expression child1 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Symbol::Builder('a'));
    Expression child2 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Symbol::Builder('b'));
    Expression e1 = MultiplicationExplicite::Builder(child2.clone(), child1.clone());
    Expression e2 = MultiplicationExplicite::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^3 * pi^2 -> pi^2 * pi^3
    Expression child1 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(2));
    Expression child2 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(3));
    Expression e1 = MultiplicationExplicite::Builder(child2.clone(), child1.clone());
    Expression e2 = MultiplicationExplicite::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 1 + Matrix1 + 2 -> 1 + 2 + Matrix1
    Expression child1 = Rational::Builder(1);
    Expression child2 = Rational::Builder(2);
    Expression childMatrix = Matrix::Builder();
    static_cast<Matrix &>(childMatrix).addChildAtIndexInPlace(Rational::Builder(3), 0, 0);
    Expression e1 = MultiplicationExplicite::Builder(child2.clone(), childMatrix.clone(), child1.clone());
    Expression e2 = MultiplicationExplicite::Builder(child1.clone(), child2.clone(), childMatrix.clone());
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

    constexpr int numberOfChildren = 4;
    Expression children[numberOfChildren] = {
      child2.clone(),
      childMatrix1.clone(),
      childMatrix2.clone(),
      child1.clone()
    };
    Expression childrenSorted[numberOfChildren] = {
      child1.clone(),
      child2.clone(),
      childMatrix1.clone(),
      childMatrix2.clone()
    };
    Expression e1 = MultiplicationExplicite::Builder(children, numberOfChildren);
    Expression e2 = MultiplicationExplicite::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

}

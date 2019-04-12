#include <quiz.h>
#include <poincare/expression.h>
#include <poincare/include/poincare_nodes.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

void assert_multiplication_or_addition_is_ordered_as(Expression e1, Expression e2) {
  if (e1.type() == ExpressionNode::Type::Multiplication) {
    static_cast<Multiplication&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, true, canBeInterrupted); },
        true);
  } else {
    assert(e1.type() == ExpressionNode::Type::Addition);
    static_cast<Addition&>(e1).sortChildrenInPlace(
        [](const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) { return ExpressionNode::SimplificationOrder(e1, e2, false, canBeInterrupted); },
        true);
  }
  quiz_assert(e1.isIdenticalTo(e2));
}

QUIZ_CASE(poincare_expression_order) {
  {
    // 2 * 5
    Expression e1 = Multiplication::Builder(Rational::Builder(5), Rational::Builder(2));
    Expression e2 = Multiplication::Builder(Rational::Builder(2), Rational::Builder(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2 + 1 + 0
    constexpr int numberOfChildren = 3;
    Expression children[numberOfChildren] = {Rational::Builder(1), Rational::Builder(2), Rational::Builder(0)};
    Expression childrenSorted[numberOfChildren] = {Rational::Builder(2), Rational::Builder(1), Rational::Builder(0)};
    Expression e1 = Addition::Builder(children, numberOfChildren);
    Expression e2 = Addition::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // e + pi + i
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
    // 2 * root(3)
    Expression e1 = Multiplication::Builder(SquareRoot::Builder(Rational::Builder(3)), Rational::Builder(2));
    Expression e2 = Multiplication::Builder(Rational::Builder(2), SquareRoot::Builder(Rational::Builder(3)));
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
    // a^2 + a + b^2 + c
    Expression e1 = Addition::Builder(children, numberOfChildren);
    Expression e2 = Addition::Builder(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2*x^3 + 3*x^2
    Expression child1 = Multiplication::Builder(Rational::Builder(2), Power::Builder(Symbol::Builder('x'), Rational::Builder(3)));
    Expression child2 = Multiplication::Builder(Rational::Builder(3), Power::Builder(Symbol::Builder('x'), Rational::Builder(2)));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 3*x + 2*x
    Expression child1 = Multiplication::Builder(Rational::Builder(3), Symbol::Builder('x'));
    Expression child2 = Multiplication::Builder(Rational::Builder(2), Symbol::Builder('x'));
    Expression e1 = Addition::Builder(child2.clone(), child1.clone());
    Expression e2 = Addition::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^a * pi^b
    Expression child1 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Symbol::Builder('a'));
    Expression child2 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Symbol::Builder('b'));
    Expression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    Expression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^2 * pi^3
    Expression child1 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(2));
    Expression child2 = Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(3));
    Expression e1 = Multiplication::Builder(child2.clone(), child1.clone());
    Expression e2 = Multiplication::Builder(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

}

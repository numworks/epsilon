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
    Expression e1 = Multiplication(Rational(5), Rational(2));
    Expression e2 = Multiplication(Rational(2), Rational(5));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2 + 1 + 0
    constexpr int numberOfChildren = 3;
    Expression children[numberOfChildren] = {Rational(1), Rational(2), Rational(0)};
    Expression childrenSorted[numberOfChildren] = {Rational(2), Rational(1), Rational(0)};
    Expression e1 = Addition(children, numberOfChildren);
    Expression e2 = Addition(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // e + pi + i
    Expression pi = Constant(Ion::Charset::SmallPi);
    Expression i = Constant(Ion::Charset::IComplex);
    Expression e = Constant(Ion::Charset::Exponential);
    constexpr int numberOfChildren = 3;
    Expression children[numberOfChildren] = {pi.clone(), i.clone(), e.clone()};
    Expression childrenSorted[numberOfChildren] = {e, pi, i};
    Expression e1 = Addition(children, numberOfChildren);
    Expression e2 = Addition(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2 * root(3)
    Expression e1 = Multiplication(SquareRoot::Builder(Rational(3)), Rational(2));
    Expression e2 = Multiplication(Rational(2), SquareRoot::Builder(Rational(3)));
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    constexpr int numberOfChildren = 4;
    Expression children[numberOfChildren] = {
      Symbol('c'),
      Power(Symbol('b'), Rational(2)),
      Power(Symbol('a'), Rational(2)),
      Symbol('a')
    };
    Expression childrenSorted[numberOfChildren] = {
      Power(Symbol('a'), Rational(2)),
      Symbol('a'),
      Power(Symbol('b'), Rational(2)),
      Symbol('c')
    };
    // a^2 + a + b^2 + c
    Expression e1 = Addition(children, numberOfChildren);
    Expression e2 = Addition(childrenSorted, numberOfChildren);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 2*x^3 + 3*x^2
    Expression child1 = Multiplication(Rational(2), Power(Symbol('x'), Rational(3)));
    Expression child2 = Multiplication(Rational(3), Power(Symbol('x'), Rational(2)));
    Expression e1 = Addition(child2.clone(), child1.clone());
    Expression e2 = Addition(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // 3*x + 2*x
    Expression child1 = Multiplication(Rational(3), Symbol('x'));
    Expression child2 = Multiplication(Rational(2), Symbol('x'));
    Expression e1 = Addition(child2.clone(), child1.clone());
    Expression e2 = Addition(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^a * pi^b
    Expression child1 = Power(Constant(Ion::Charset::SmallPi), Symbol('a'));
    Expression child2 = Power(Constant(Ion::Charset::SmallPi), Symbol('b'));
    Expression e1 = Multiplication(child2.clone(), child1.clone());
    Expression e2 = Multiplication(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }
  {
    // pi^2 * pi^3
    Expression child1 = Power(Constant(Ion::Charset::SmallPi), Rational(2));
    Expression child2 = Power(Constant(Ion::Charset::SmallPi), Rational(3));
    Expression e1 = Multiplication(child2.clone(), child1.clone());
    Expression e2 = Multiplication(child1, child2);
    assert_multiplication_or_addition_is_ordered_as(e1, e2);
  }

}

#include <quiz.h>
#include <poincare_layouts.h>
#include <apps/shared/global_context.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

void assert_parsed_layout_is(Layout l, Poincare::Expression r) {
  Shared::GlobalContext context;
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);
  Expression e = parse_expression(buffer);
  Expression eSimplified = e.clone().simplify(context, Preferences::AngleUnit::Degree);
  if (eSimplified.isUninitialized()) {
    /* In case the simplification is impossible (if there are matrices for
     * instance), use the non simplified expression */
    eSimplified = e;
  }
  Expression rSimplified = r.clone().simplify(context, Preferences::AngleUnit::Degree);
  if (rSimplified.isUninitialized()) {
    /* In case the simplification is impossible (if there are matrices for
     * instance), use the non simplified expression */
    rSimplified = r;
  }

  bool identical = eSimplified.isIdenticalTo(rSimplified);
#if POINCARE_TREE_LOG
  if (!identical) {
    std::cout << "Expecting" << std::endl;
    rSimplified.log();
    std::cout << "Got" << std::endl;
    eSimplified.log();
  }
#endif
  quiz_assert(identical);
}

QUIZ_CASE(poincare_create_all_layouts) {
  EmptyLayout e0;
  AbsoluteValueLayout e1(e0);
  CharLayout e2('a');
  BinomialCoefficientLayout e3(e1, e2);
  CeilingLayout e4(e3);
  RightParenthesisLayout e5;
  RightSquareBracketLayout e6;
  CondensedSumLayout e7(e4, e5, e6);
  ConjugateLayout e8(e7);
  LeftParenthesisLayout e10;
  FloorLayout e11(e10);
  FractionLayout e12(e8, e11);
  HorizontalLayout e13;
  LeftSquareBracketLayout e14;
  IntegralLayout e15(e11, e12, e13, e14);
  NthRootLayout e16(e15);
  MatrixLayout e17;
  EmptyLayout e18;
  EmptyLayout e19;
  EmptyLayout e20;
  ProductLayout e21(e17, e18, e19, e20);
  EmptyLayout e22;
  EmptyLayout e23;
  EmptyLayout e24;
  SumLayout e25(e21, e22, e23, e24);
  VerticalOffsetLayout e26(e25, VerticalOffsetLayoutNode::Type::Superscript);
}

QUIZ_CASE(poincare_parse_layouts) {
  Layout l;
  Expression e;

  // 1+2
  l = HorizontalLayout(
      CharLayout('1'),
      CharLayout('+'),
      CharLayout('2'));
  e = Addition(Rational(1), Rational(2));
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = AbsoluteValueLayout(
      HorizontalLayout(
        CharLayout('3'),
        CharLayout('+'),
        FractionLayout(
          CharLayout('3'),
          CharLayout('6'))));
  e = AbsoluteValue::Builder(
      Addition(
        Rational(3),
        Division(
          Rational(3),
          Rational(6))));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = BinomialCoefficientLayout(
      CharLayout('4'),
      CharLayout('5'));
  e = BinomialCoefficient::Builder(
      Rational(4),
      Rational(5));
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = CeilingLayout(
      HorizontalLayout(
        CharLayout('4'),
        CharLayout('.'),
        CharLayout('6')));
  e = Ceiling::Builder(
      Decimal(4.6));
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = FloorLayout(
      HorizontalLayout(
        CharLayout('7'),
        CharLayout('.'),
        CharLayout('2')));
  e = Floor::Builder(
      Decimal(7.2));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = HorizontalLayout(
      CharLayout('2'),
      VerticalOffsetLayout(
        HorizontalLayout(
          CharLayout('3'),
          CharLayout('+'),
          CharLayout('4')),
        VerticalOffsetLayoutNode::Type::Superscript));
  e = Power(
      Rational(2),
      Addition(
        Rational(3),
        Rational(4)));
  assert_parsed_layout_is(l, e);

  // log_3(2)
  HorizontalLayout l1 = HorizontalLayout();
  l1.addChildAtIndex(CharLayout('l'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('o'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('g'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(VerticalOffsetLayout(CharLayout('3'), VerticalOffsetLayoutNode::Type::Subscript), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(LeftParenthesisLayout(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('2'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(RightParenthesisLayout(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l = l1;
  e = Logarithm::Builder(
      Rational(2),
      Rational(3));
  assert_parsed_layout_is(l, e);

  // root(5,3)
  l = NthRootLayout(
      CharLayout('5'),
      CharLayout('3'));
  e = NthRoot::Builder(Rational(5), Rational(3));
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = IntegralLayout(
      CharLayout('7'),
      CharLayout('x'),
      CharLayout('4'),
      CharLayout('5'));
  e = Integral::Builder(
      Rational(7),
      Symbol('x'),
      Rational(4),
      Rational(5));
  assert_parsed_layout_is(l, e);

  // 2^2 !
  l = HorizontalLayout(
      CharLayout('2'),
      VerticalOffsetLayout(
        CharLayout('2'),
        VerticalOffsetLayoutNode::Type::Superscript),
      CharLayout('!'));
  e = Factorial(
      Power(
        Rational(2),
        Rational(2)));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = HorizontalLayout(
      CharLayout('5'),
      FractionLayout(
        CharLayout('6'),
        HorizontalLayout(
          CharLayout('7'),
          CharLayout('+'),
          CharLayout('5'))),
      CharLayout('3'));
  e = Multiplication(
      Rational(5),
      Division(
        Rational(6),
        Addition(
          Rational(7),
          Rational(5))),
      Rational(3));
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = MatrixLayout(
      HorizontalLayout(
        CharLayout('3'),
        VerticalOffsetLayout(
          CharLayout('2'),
          VerticalOffsetLayoutNode::Type::Superscript),
        CharLayout('!')),
      CharLayout('7'),
      CharLayout('4'),
      CharLayout('5'));
  Matrix m = Matrix(
      Factorial(
        Power(
          Rational(3),
          Rational(2))));
  m.addChildAtIndexInPlace(Rational(7), 1, 1);
  m.addChildAtIndexInPlace(Rational(4), 2, 2);
  m.addChildAtIndexInPlace(Rational(5), 3, 3);
  m.setDimensions(2,2);
  e = m;
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = HorizontalLayout(
      CharLayout('2'),
      VerticalOffsetLayout(
        MatrixLayout(
          HorizontalLayout(
            CharLayout('3'),
            CharLayout('!')),
          CharLayout('7'),
          CharLayout('4'),
          CharLayout('5')),
        VerticalOffsetLayoutNode::Type::Superscript));
  m = Matrix(
      Factorial(
        Rational(3)));
  m.addChildAtIndexInPlace(Rational(7), 1, 1);
  m.addChildAtIndexInPlace(Rational(4), 2, 2);
  m.addChildAtIndexInPlace(Rational(5), 3, 3);
  m.setDimensions(2,2);
  e = Power(Rational(2), m);
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = HorizontalLayout(
      CharLayout('2'),
      CharLayout(Ion::Charset::Exponential),
      VerticalOffsetLayout(
        CharLayout('3'),
        VerticalOffsetLayoutNode::Type::Superscript));
  e = Multiplication(Rational(2),Power(Constant(Ion::Charset::Exponential),Parenthesis(Rational(3))));
  assert_parsed_expression_is("2X^(3)", Multiplication(Rational(2),Power(Constant(Ion::Charset::Exponential),Parenthesis(Rational(3)))));
  assert_parsed_layout_is(l, e);
}

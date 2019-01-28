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
  Expression eSimplified;
  e.clone().simplifyAndApproximate(&eSimplified, nullptr, context, Cartesian, Degree);
  if (eSimplified.isUninitialized()) {
    /* In case the simplification is impossible (if there are matrices for
     * instance), use the non simplified expression */
    eSimplified = e;
  }
  Expression rSimplified;
  r.clone().simplifyAndApproximate(&rSimplified, nullptr, context, Cartesian, Degree);
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
  AbsoluteValueLayout e1 = AbsoluteValueLayout::Builder(e0);
  CharLayout e2('a');
  BinomialCoefficientLayout e3 = BinomialCoefficientLayout::Builder(e1, e2);
  CeilingLayout e4 = CeilingLayout::Builder(e3);
  RightParenthesisLayout e5 = RightParenthesisLayout::Builder();
  RightSquareBracketLayout e6 = RightSquareBracketLayout::Builder();
  CondensedSumLayout e7 = CondensedSumLayout::Builder(e4, e5, e6);
  ConjugateLayout e8 = ConjugateLayout::Builder(e7);
  LeftParenthesisLayout e10 = LeftParenthesisLayout::Builder();
  FloorLayout e11 = FloorLayout::Builder(e10);
  FractionLayout e12 = FractionLayout::Builder(e8, e11);
  HorizontalLayout e13 = HorizontalLayout::Builder();
  LeftSquareBracketLayout e14 = LeftSquareBracketLayout::Builder();
  IntegralLayout e15 = IntegralLayout::Builder(e11, e12, e13, e14);
  NthRootLayout e16 = NthRootLayout::Builder(e15);
  MatrixLayout e17 = MatrixLayout::Builder();
  EmptyLayout e18;
  EmptyLayout e19;
  EmptyLayout e20;
  ProductLayout e21 = ProductLayout::Builder(e17, e18, e19, e20);
  EmptyLayout e22;
  EmptyLayout e23;
  EmptyLayout e24;
  SumLayout e25 = SumLayout::Builder(e21, e22, e23, e24);
  VerticalOffsetLayout e26 = VerticalOffsetLayout::Builder(e25, VerticalOffsetLayoutNode::Type::Superscript);
}

Matrix BuildOneChildMatrix(Expression entry) {
  Matrix m = Matrix::Builder();
  m.addChildAtIndexInPlace(entry, 0, 0);
  return m;
}

QUIZ_CASE(poincare_parse_layouts) {
  Layout l;
  Expression e;

  // 1+2
  l = HorizontalLayout::Builder(
      CharLayout('1'),
      CharLayout('+'),
      CharLayout('2'));
  e = Addition::Builder(Rational(1), Rational(2));
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = AbsoluteValueLayout:: Builder(
      HorizontalLayout::Builder(
        CharLayout('3'),
        CharLayout('+'),
        FractionLayout::Builder(
          CharLayout('3'),
          CharLayout('6'))));
  e = AbsoluteValue::Builder(
      Addition::Builder(
        Rational(3),
        Division::Builder(
          Rational(3),
          Rational(6))));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = BinomialCoefficientLayout::Builder(
      CharLayout('4'),
      CharLayout('5'));
  e = BinomialCoefficient::Builder(
      Rational(4),
      Rational(5));
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = CeilingLayout::Builder(
      HorizontalLayout::Builder(
        CharLayout('4'),
        CharLayout('.'),
        CharLayout('6')));
  e = Ceiling::Builder(
      Decimal(4.6));
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = FloorLayout::Builder(
      HorizontalLayout::Builder(
        CharLayout('7'),
        CharLayout('.'),
        CharLayout('2')));
  e = Floor::Builder(
      Decimal(7.2));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = HorizontalLayout::Builder(
      CharLayout('2'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(
          CharLayout('3'),
          CharLayout('+'),
          CharLayout('4')),
        VerticalOffsetLayoutNode::Type::Superscript));
  e = Power::Builder(
      Rational(2),
      Addition::Builder(
        Rational(3),
        Rational(4)));
  assert_parsed_layout_is(l, e);

  // log_3(2)
  HorizontalLayout l1 = HorizontalLayout::Builder();
  l1.addChildAtIndex(CharLayout('l'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('o'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('g'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(VerticalOffsetLayout::Builder(CharLayout('3'), VerticalOffsetLayoutNode::Type::Subscript), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(LeftParenthesisLayout::Builder(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CharLayout('2'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(RightParenthesisLayout::Builder(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l = l1;
  e = Logarithm::Builder(
      Rational(2),
      Rational(3));
  assert_parsed_layout_is(l, e);

  // root(5,3)
  l = NthRootLayout::Builder(
      CharLayout('5'),
      CharLayout('3'));
  e = NthRoot::Builder(Rational(5), Rational(3));
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = IntegralLayout::Builder(
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
  l = HorizontalLayout::Builder(
      CharLayout('2'),
      VerticalOffsetLayout::Builder(
        CharLayout('2'),
        VerticalOffsetLayoutNode::Type::Superscript),
      CharLayout('!'));
  e = Factorial::Builder(
      Power::Builder(
        Rational(2),
        Rational(2)));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = HorizontalLayout::Builder(
      CharLayout('5'),
      FractionLayout::Builder(
        CharLayout('6'),
        HorizontalLayout::Builder(
          CharLayout('7'),
          CharLayout('+'),
          CharLayout('5'))),
      CharLayout('3'));
  e = Multiplication::Builder(
      Rational(5),
      Division::Builder(
        Rational(6),
        Addition::Builder(
          Rational(7),
          Rational(5))),
      Rational(3));
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = MatrixLayout::Builder(
      HorizontalLayout::Builder(
        CharLayout('3'),
        VerticalOffsetLayout::Builder(
          CharLayout('2'),
          VerticalOffsetLayoutNode::Type::Superscript),
        CharLayout('!')),
      CharLayout('7'),
      CharLayout('4'),
      CharLayout('5'));
  Matrix m = BuildOneChildMatrix(
      Factorial::Builder(
        Power::Builder(
          Rational(3),
          Rational(2))));
  m.addChildAtIndexInPlace(Rational(7), 1, 1);
  m.addChildAtIndexInPlace(Rational(4), 2, 2);
  m.addChildAtIndexInPlace(Rational(5), 3, 3);
  m.setDimensions(2,2);
  e = m;
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = HorizontalLayout::Builder(
      CharLayout('2'),
      VerticalOffsetLayout::Builder(
        MatrixLayout::Builder(
          HorizontalLayout::Builder(
            CharLayout('3'),
            CharLayout('!')),
          CharLayout('7'),
          CharLayout('4'),
          CharLayout('5')),
        VerticalOffsetLayoutNode::Type::Superscript));
  m = BuildOneChildMatrix(
      Factorial::Builder(
        Rational(3)));
  m.addChildAtIndexInPlace(Rational(7), 1, 1);
  m.addChildAtIndexInPlace(Rational(4), 2, 2);
  m.addChildAtIndexInPlace(Rational(5), 3, 3);
  m.setDimensions(2,2);
  e = Power::Builder(Rational(2), m);
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = HorizontalLayout::Builder(
      CharLayout('2'),
      CharLayout(Ion::Charset::Exponential),
      VerticalOffsetLayout::Builder(
        CharLayout('3'),
        VerticalOffsetLayoutNode::Type::Superscript));
  e = Multiplication::Builder(Rational(2),Power::Builder(Constant(Ion::Charset::Exponential),Parenthesis::Builder(Rational(3))));
  assert_parsed_expression_is("2X^(3)", Multiplication::Builder(Rational(2),Power::Builder(Constant(Ion::Charset::Exponential),Parenthesis::Builder(Rational(3)))));
  assert_parsed_layout_is(l, e);
}

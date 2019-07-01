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
  e.clone().simplifyAndApproximate(&eSimplified, nullptr, &context, Cartesian, Degree);
  if (eSimplified.isUninitialized()) {
    /* In case the simplification is impossible (if there are matrices for
     * instance), use the non simplified expression */
    eSimplified = e;
  }
  Expression rSimplified;
  r.clone().simplifyAndApproximate(&rSimplified, nullptr, &context, Cartesian, Degree);
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

void assert_layout_is_not_parsed(Layout l) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serializeForParsing(buffer, bufferSize);
  Expression e = parse_expression(buffer, true);
  quiz_assert(e.isUninitialized());
}

QUIZ_CASE(poincare_create_all_layouts) {
  EmptyLayout e0 = EmptyLayout::Builder();
  AbsoluteValueLayout e1 = AbsoluteValueLayout::Builder(e0);
  CodePointLayout e2 = CodePointLayout::Builder('a');
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
  EmptyLayout e18 = EmptyLayout::Builder();
  EmptyLayout e19 = EmptyLayout::Builder();
  EmptyLayout e20 = EmptyLayout::Builder();
  ProductLayout e21 = ProductLayout::Builder(e17, e18, e19, e20);
  EmptyLayout e22 = EmptyLayout::Builder();
  EmptyLayout e23 = EmptyLayout::Builder();
  EmptyLayout e24 = EmptyLayout::Builder();
  SumLayout e25 = SumLayout::Builder(e21, e22, e23, e24);
  VerticalOffsetLayout e26 = VerticalOffsetLayout::Builder(e25, VerticalOffsetLayoutNode::Position::Superscript);
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
      CodePointLayout::Builder('1'),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('2'));
  e = Addition::Builder(Rational::Builder(1), Rational::Builder(2));
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = AbsoluteValueLayout:: Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('3'),
        CodePointLayout::Builder('+'),
        FractionLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('6'))));
  e = AbsoluteValue::Builder(
      Addition::Builder(
        Rational::Builder(3),
        Division::Builder(
          Rational::Builder(3),
          Rational::Builder(6))));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = BinomialCoefficientLayout::Builder(
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  e = BinomialCoefficient::Builder(
      Rational::Builder(4),
      Rational::Builder(5));
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = CeilingLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('4'),
        CodePointLayout::Builder('.'),
        CodePointLayout::Builder('6')));
  e = Ceiling::Builder(
      Decimal::Builder(4.6));
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = FloorLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('7'),
        CodePointLayout::Builder('.'),
        CodePointLayout::Builder('2')));
  e = Floor::Builder(
      Decimal::Builder(7.2));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('4')),
        VerticalOffsetLayoutNode::Position::Superscript));
  e = Power::Builder(
      Rational::Builder(2),
      Addition::Builder(
        Rational::Builder(3),
        Rational::Builder(4)));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)!
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(
          CodePointLayout::Builder('3'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('4')),
        VerticalOffsetLayoutNode::Position::Superscript),
      CodePointLayout::Builder('!'));
  e = Factorial::Builder(
      Power::Builder(
        Rational::Builder(2),
        Addition::Builder(
          Rational::Builder(3),
          Rational::Builder(4))));
  assert_parsed_layout_is(l, e);


  // log_3(2)
  HorizontalLayout l1 = HorizontalLayout::Builder();
  l1.addChildAtIndex(CodePointLayout::Builder('l'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CodePointLayout::Builder('o'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CodePointLayout::Builder('g'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(VerticalOffsetLayout::Builder(CodePointLayout::Builder('3'), VerticalOffsetLayoutNode::Position::Subscript), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(LeftParenthesisLayout::Builder(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(CodePointLayout::Builder('2'), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l1.addChildAtIndex(RightParenthesisLayout::Builder(), l1.numberOfChildren(),  l1.numberOfChildren(), nullptr);
  l = l1;
  e = Logarithm::Builder(
      Rational::Builder(2),
      Rational::Builder(3));
  assert_parsed_layout_is(l, e);

  // root(5,3)
  l = NthRootLayout::Builder(
      CodePointLayout::Builder('5'),
      CodePointLayout::Builder('3'));
  e = NthRoot::Builder(Rational::Builder(5), Rational::Builder(3));
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = IntegralLayout::Builder(
      CodePointLayout::Builder('7'),
      CodePointLayout::Builder('x'),
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  e = Integral::Builder(
      Rational::Builder(7),
      Symbol::Builder('x'),
      Rational::Builder(4),
      Rational::Builder(5));
  assert_parsed_layout_is(l, e);

  // 2^2 !
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayoutNode::Position::Superscript),
      CodePointLayout::Builder('!'));
  e = Factorial::Builder(
      Power::Builder(
        Rational::Builder(2),
        Rational::Builder(2)));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('5'),
      FractionLayout::Builder(
        CodePointLayout::Builder('6'),
        HorizontalLayout::Builder(
          CodePointLayout::Builder('7'),
          CodePointLayout::Builder('+'),
          CodePointLayout::Builder('5'))),
      CodePointLayout::Builder('3'));
  e = Multiplication::Builder(
      Rational::Builder(5),
      Division::Builder(
        Rational::Builder(6),
        Addition::Builder(
          Rational::Builder(7),
          Rational::Builder(5))),
      Rational::Builder(3));
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = MatrixLayout::Builder(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('3'),
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('2'),
          VerticalOffsetLayoutNode::Position::Superscript),
        CodePointLayout::Builder('!')),
      CodePointLayout::Builder('7'),
      CodePointLayout::Builder('4'),
      CodePointLayout::Builder('5'));
  Matrix m = BuildOneChildMatrix(
      Factorial::Builder(
        Power::Builder(
          Rational::Builder(3),
          Rational::Builder(2))));
  m.addChildAtIndexInPlace(Rational::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(Rational::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(Rational::Builder(5), 3, 3);
  m.setDimensions(2,2);
  e = m;
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        MatrixLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('3'),
            CodePointLayout::Builder('!')),
          CodePointLayout::Builder('7'),
          CodePointLayout::Builder('4'),
          CodePointLayout::Builder('5')),
        VerticalOffsetLayoutNode::Position::Superscript));
  m = BuildOneChildMatrix(
      Factorial::Builder(
        Rational::Builder(3)));
  m.addChildAtIndexInPlace(Rational::Builder(7), 1, 1);
  m.addChildAtIndexInPlace(Rational::Builder(4), 2, 2);
  m.addChildAtIndexInPlace(Rational::Builder(5), 3, 3);
  m.setDimensions(2,2);
  e = Power::Builder(Rational::Builder(2), m);
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      CodePointLayout::Builder(UCodePointScriptSmallE),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('3'),
        VerticalOffsetLayoutNode::Position::Superscript));
  e = Multiplication::Builder(Rational::Builder(2),Power::Builder(Constant::Builder(UCodePointScriptSmallE),Parenthesis::Builder(Rational::Builder(3))));
  assert_parsed_expression_is("2ℯ^(3)", Multiplication::Builder(Rational::Builder(2),Power::Builder(Constant::Builder(UCodePointScriptSmallE),Parenthesis::Builder(Rational::Builder(3)))));
  assert_parsed_layout_is(l, e);
}

QUIZ_CASE(poincare_layouts_comparison) {
  Layout e0 = CodePointLayout::Builder('a');
  Layout e1 = CodePointLayout::Builder('a');
  Layout e2 = CodePointLayout::Builder('b');
  quiz_assert(e0.isIdenticalTo(e1));
  quiz_assert(!e0.isIdenticalTo(e2));

  Layout e3 = EmptyLayout::Builder();
  Layout e4 = EmptyLayout::Builder();
  quiz_assert(e3.isIdenticalTo(e4));
  quiz_assert(!e3.isIdenticalTo(e0));

  Layout e5 = NthRootLayout::Builder(e0);
  Layout e6 = NthRootLayout::Builder(e1);
  Layout e7 = NthRootLayout::Builder(e2);
  quiz_assert(e5.isIdenticalTo(e6));
  quiz_assert(!e5.isIdenticalTo(e7));
  quiz_assert(!e5.isIdenticalTo(e0));

  Layout e8 = VerticalOffsetLayout::Builder(e5, VerticalOffsetLayoutNode::Position::Superscript);
  Layout e9 = VerticalOffsetLayout::Builder(e6, VerticalOffsetLayoutNode::Position::Superscript);
  Layout e10 = VerticalOffsetLayout::Builder(NthRootLayout::Builder(CodePointLayout::Builder('a')), VerticalOffsetLayoutNode::Position::Subscript);
  quiz_assert(e8.isIdenticalTo(e9));
  quiz_assert(!e8.isIdenticalTo(e10));
  quiz_assert(!e8.isIdenticalTo(e0));

  Layout e11 = SumLayout::Builder(e0, e3, e6, e2);
  Layout e12 = SumLayout::Builder(CodePointLayout::Builder('a'), EmptyLayout::Builder(), NthRootLayout::Builder(CodePointLayout::Builder('a')), CodePointLayout::Builder('b'));
  Layout e13 = ProductLayout::Builder(CodePointLayout::Builder('a'), EmptyLayout::Builder(), NthRootLayout::Builder(CodePointLayout::Builder('a')), CodePointLayout::Builder('b'));
  quiz_assert(e11.isIdenticalTo(e12));
  quiz_assert(!e11.isIdenticalTo(e13));
}

QUIZ_CASE(poincare_unparsable_layouts) {
  {
    /*  (1
     *  ---
     *   2)
     * */
    Layout l = FractionLayout::Builder(
        HorizontalLayout::Builder(
          LeftParenthesisLayout::Builder(),
          CodePointLayout::Builder('1')),
        HorizontalLayout::Builder(
          CodePointLayout::Builder('2'),
          RightParenthesisLayout::Builder()));
    assert_layout_is_not_parsed(l);
  }

  {
    //  (  √2)
    Layout l = HorizontalLayout::Builder(
        LeftParenthesisLayout::Builder(),
        NthRootLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('2'),
            RightParenthesisLayout::Builder())));
    assert_layout_is_not_parsed(l);
  }

  {
    /*   2)+(1
     *    ∑     (5)
     *   n=1
     */
    constexpr int childrenCount = 5;
    Layout children[childrenCount] = {
      CodePointLayout::Builder('2'),
      RightParenthesisLayout::Builder(),
      CodePointLayout::Builder('+'),
      LeftParenthesisLayout::Builder(),
      CodePointLayout::Builder('1')};

    Layout l = SumLayout::Builder(
        CodePointLayout::Builder('5'),
        CodePointLayout::Builder('n'),
        CodePointLayout::Builder('1'),
        HorizontalLayout::Builder(children, childrenCount));
    assert_layout_is_not_parsed(l);
  }

  {
    /*   2)+(1
     *    π    (5)
     *   n=1
     */
    constexpr int childrenCount = 5;
    Layout children[childrenCount] = {
      CodePointLayout::Builder('2'),
      RightParenthesisLayout::Builder(),
      CodePointLayout::Builder('+'),
      LeftParenthesisLayout::Builder(),
      CodePointLayout::Builder('1')};

    Layout l = ProductLayout::Builder(
        CodePointLayout::Builder('5'),
        CodePointLayout::Builder('n'),
        CodePointLayout::Builder('1'),
        HorizontalLayout::Builder(children, childrenCount));
    assert_layout_is_not_parsed(l);
  }

  {
    /*   2)+(1
     *    ∫    (5)dx
     *    1
     */
    constexpr int childrenCount = 5;
    Layout children[childrenCount] = {
      CodePointLayout::Builder('2'),
      RightParenthesisLayout::Builder(),
      CodePointLayout::Builder('+'),
      LeftParenthesisLayout::Builder(),
      CodePointLayout::Builder('1')};

    Layout l = ProductLayout::Builder(
        CodePointLayout::Builder('5'),
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder('1'),
        HorizontalLayout::Builder(children, childrenCount));
    assert_layout_is_not_parsed(l);
  }

  {
    // |3)+(1|
    constexpr int childrenCount = 5;
    Layout children[childrenCount] = {
      CodePointLayout::Builder('3'),
      RightParenthesisLayout::Builder(),
      CodePointLayout::Builder('+'),
      LeftParenthesisLayout::Builder(),
      CodePointLayout::Builder('1')};

    Layout l = AbsoluteValueLayout::Builder(HorizontalLayout::Builder(children, childrenCount));
    assert_layout_is_not_parsed(l);
  }
}

#include <quiz.h>
#include <poincare_layouts.h>
#include "helper.h"

using namespace Poincare;

void assert_inserted_layout_points_to(Layout layoutToInsert, Expression * correspondingExpression, Layout layoutToPoint) {
  quiz_assert(correspondingExpression != nullptr);
  assert_expression_layouts_as(*correspondingExpression, layoutToInsert);
  Layout layoutToPointComputed = layoutToInsert.layoutToPointWhenInserting(correspondingExpression);
  quiz_assert(layoutToPointComputed.identifier() == layoutToPoint.identifier());
}

QUIZ_CASE(poincare_layout_cursor_computation) {
  Layout l;
  Expression e;

  // 1+2
  l = HorizontalLayout::Builder(
      CodePointLayout::Builder('1'),
      CodePointLayout::Builder('+'),
      CodePointLayout::Builder('2'));
  e = Addition::Builder(Rational::Builder(1), Rational::Builder(2));
  assert_inserted_layout_points_to(l, &e, l);

  // random()
  HorizontalLayout hl = HorizontalLayout::Builder();
  hl.addChildAtIndex(CodePointLayout::Builder('r'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('a'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('n'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('d'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('o'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('m'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(LeftParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(RightParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  l = hl;
  e = Random::Builder();
  assert_inserted_layout_points_to(l, &e, l);

  // cos(\x11)
  hl = HorizontalLayout::Builder();
  hl.addChildAtIndex(CodePointLayout::Builder('c'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('o'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(CodePointLayout::Builder('s'), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(LeftParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(RightParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  l = hl;
  e = Cosine::Builder(EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(3));

  // •^•
  l = HorizontalLayout::Builder(
      EmptyLayout::Builder(),
      VerticalOffsetLayout::Builder(
        EmptyLayout::Builder(),
        VerticalOffsetLayoutNode::Position::Superscript)
      );
  e = Power::Builder(EmptyExpression::Builder(), EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(0));

  // int(•, x, •, •)
  l = IntegralLayout::Builder(
      EmptyLayout::Builder(),
      HorizontalLayout::Builder(
        CodePointLayout::Builder('x')),
      EmptyLayout::Builder(),
      EmptyLayout::Builder());
  e = Integral::Builder(
      EmptyExpression::Builder(),
      Symbol::Builder('x'),
      EmptyExpression::Builder(),
      EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(2));
}

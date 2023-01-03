#include <poincare_layouts.h>
#include <poincare/addition.h>
#include <poincare/cosine.h>
#include <poincare/empty_expression.h>
#include <poincare/integral.h>
#include <poincare/power.h>
#include <poincare/random.h>
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
  hl.addChildAtIndex(StringLayout::Builder("random"), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(ParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  l = hl;
  e = Random::Builder();
  assert_inserted_layout_points_to(l, &e, l);

  // cos(\x11)
  hl = HorizontalLayout::Builder();
  hl.addChildAtIndex(StringLayout::Builder("cos"), hl.numberOfChildren(),  hl.numberOfChildren(), nullptr);
  hl.addChildAtIndex(ParenthesisLayout::Builder(), hl.numberOfChildren(), hl.numberOfChildren(), nullptr);
  l = hl;
  e = Cosine::Builder(EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(1).childAtIndex(0));

  // ▯^▯
  l = HorizontalLayout::Builder(
      EmptyLayout::Builder(),
      VerticalOffsetLayout::Builder(
        EmptyLayout::Builder(),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript)
      );
  e = Power::Builder(EmptyExpression::Builder(), EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(0));

  // int(▯, x, ▯, ▯)
  l = IntegralLayout::Builder(
      EmptyLayout::Builder(),
      CodePointLayout::Builder('x'),
      EmptyLayout::Builder(),
      EmptyLayout::Builder());
  e = Integral::Builder(
      EmptyExpression::Builder(),
      Symbol::Builder('x'),
      EmptyExpression::Builder(),
      EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, &e, l.childAtIndex(2));
}

QUIZ_CASE(poincare_layout_cursor_delete) {

  /*  12
   * --- -> "BackSpace" -> 12|34
   * |34
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        FractionLayout::Builder(
          LayoutHelper::StringToCodePointsLayout("12", 2),
          LayoutHelper::StringToCodePointsLayout("34", 2)
          )
        );
    LayoutCursor cursor(layout.childAtIndex(0).childAtIndex(1), LayoutCursor::Position::Left);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1234");
    quiz_assert(cursor.isEquivalentTo(LayoutCursor(layout.childAtIndex(1), LayoutCursor::Position::Right)));
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|3
   *     |3
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('+'),
        FractionLayout::Builder(
          EmptyLayout::Builder(),
          CodePointLayout::Builder('3')
          )
        );
    LayoutCursor cursor(layout.childAtIndex(2).childAtIndex(1), LayoutCursor::Position::Left);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1+3");
    quiz_assert(cursor.isEquivalentTo(LayoutCursor(layout.childAtIndex(1), LayoutCursor::Position::Right)));
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|^2
   *     |^2
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('+'),
        FractionLayout::Builder(
          EmptyLayout::Builder(),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'), VerticalOffsetLayoutNode::VerticalPosition::Superscript)
          )
        );
    LayoutCursor cursor(layout.childAtIndex(2).childAtIndex(1), LayoutCursor::Position::Left);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1+^\u00122\u0013");
  }

  /*
   * [[|1,▯][▯,▯]] -> "Backspace" -> |1
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        MatrixLayout::Builder(
          CodePointLayout::Builder('1'),
          EmptyLayout::Builder(),
          EmptyLayout::Builder(),
          EmptyLayout::Builder()
          ));
    LayoutCursor cursor(layout.childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Left);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1");
    quiz_assert(cursor.isEquivalentTo(LayoutCursor(layout.childAtIndex(0), LayoutCursor::Position::Left)));
  }

  /* (I is the cursor in this comment since | is used for abs())
   * 2|I1|+3 -> "Backspace" -> 2I1+3
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('2'),
        AbsoluteValueLayout::Builder(
          CodePointLayout::Builder('1')
          ),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('3'));
    LayoutCursor cursor(layout.childAtIndex(1).childAtIndex(0), LayoutCursor::Position::Left);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "21+3");
    quiz_assert(cursor.isEquivalentTo(LayoutCursor(layout.childAtIndex(1), LayoutCursor::Position::Left)));
  }
}

QUIZ_CASE(poincare_layout_parentheses) {
  /*
   * |∅ -> "(" -> ")" -> ()|
   */
  {
    Layout l = HorizontalLayout::Builder(EmptyLayout::Builder());
    LayoutCursor c(l, LayoutCursor::Position::Right);
    c.insertText("(", nullptr);
    c.insertText(")", nullptr);
    assert_layout_serialize_to(l, "()");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l, LayoutCursor::Position::Right)));
  }

  /*
   * |∅ -> ")" -> "(" -> ()(|)
   */
  {
    Layout l = HorizontalLayout::Builder(EmptyLayout::Builder());
    LayoutCursor c(l, LayoutCursor::Position::Right);
    c.insertText(")", nullptr);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "()()");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(1).childAtIndex(0), LayoutCursor::Position::Left)));
  }

  /*
   * |∅ -> "(" -> "(" -> ((|))
   */
  {
    Layout l = HorizontalLayout::Builder(EmptyLayout::Builder());
    LayoutCursor c(l, LayoutCursor::Position::Right);
    c.insertText("(", nullptr);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "(())");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Left)));
  }

  /*
   * 12|345 -> "(" -> 12(|345)
   */
  {
    Layout l = HorizontalLayout::Builder({ CodePointLayout::Builder('1'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3'), CodePointLayout::Builder('4'), CodePointLayout::Builder('5') });
    LayoutCursor c(l.childAtIndex(1), LayoutCursor::Position::Right);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "12(345)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(2).childAtIndex(0), LayoutCursor::Position::Left)));
  }

  /*
   * 2|^3 -> "(" -> 2(|▯^3)  ▯ == empty layout
   */
  {
    Layout l = HorizontalLayout::Builder(CodePointLayout::Builder('2'), VerticalOffsetLayout::Builder(CodePointLayout::Builder('3'), VerticalOffsetLayoutNode::VerticalPosition::Superscript));
    LayoutCursor c(l.childAtIndex(0), LayoutCursor::Position::Right);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "2(^\u00123\u0013)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(1).childAtIndex(0), LayoutCursor::Position::Left)));
  }

  /*
   * (123)| -> BACKSPACE -> (123|)
   * */
  {
    Layout l = HorizontalLayout::Builder(ParenthesisLayout::Builder(HorizontalLayout::Builder({ CodePointLayout::Builder('1'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3') })));
    LayoutCursor c(l, LayoutCursor::Position::Right);
    c.performBackspace();
    assert_layout_serialize_to(l, "(123)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Right)));
  }

  /*
   * (12(|34)5) -> BACKSPACE -> ((12|34)5)
   */
  {
    Layout l = HorizontalLayout::Builder(ParenthesisLayout::Builder(HorizontalLayout::Builder({
            CodePointLayout::Builder('1'),
            CodePointLayout::Builder('2'),
            ParenthesisLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('3'), CodePointLayout::Builder('4'))),
            CodePointLayout::Builder('5'),
            })));
    LayoutCursor c(l.childAtIndex(0).childAtIndex(0).childAtIndex(2).childAtIndex(0), LayoutCursor::Position::Left);
    c.performBackspace();
    assert_layout_serialize_to(l, "((1234)5)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(2), LayoutCursor::Position::Left)));
  }
  /*
   * ((|3)] -> BACKSPACE -> (|3)
   */
  {
    Layout l = HorizontalLayout::Builder(
      ParenthesisLayout::Builder(HorizontalLayout::Builder(
        ParenthesisLayout::Builder(HorizontalLayout::Builder(
            CodePointLayout::Builder('3'))))));
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, true);
    LayoutCursor c(l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Left);
    c.performBackspace();
    assert_layout_serialize_to(l, "(3)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Left)));
  }
  /*
   * sqrt((3]|) -> ")" -> sqrt((3))
   */
  {
    Layout l = HorizontalLayout::Builder(
      NthRootLayout::Builder(HorizontalLayout::Builder(
        ParenthesisLayout::Builder(HorizontalLayout::Builder(
            CodePointLayout::Builder('3'))))));
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).childAtIndex(0).childAtIndex(0).node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, true);
    LayoutCursor c(l.childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Right);
    c.insertText(")", nullptr);
    assert_layout_serialize_to(l, "√\u0012(3)\u0013");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0).childAtIndex(0), LayoutCursor::Position::Right)));
  }
  /*
   * |[} -> "{}" -> {}|{}
   */
  {
    Layout l = HorizontalLayout::Builder(
      CurlyBraceLayout::Builder());
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l.childAtIndex(0), LayoutCursor::Position::Left);
    c.addLayoutAndMoveCursor(CurlyBraceLayout::Builder(), nullptr);
    assert_layout_serialize_to(l, "{}{}");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(1), LayoutCursor::Position::Left)));
  }
  /*
   * [▯§▯)| -> BACKSPACE -> ▯§▯|  ▯ == empty layout, § == prefix superscript
   */
  {
    Layout l = HorizontalLayout::Builder(
      ParenthesisLayout::Builder(HorizontalLayout::Builder(
        VerticalOffsetLayout::Builder(EmptyLayout::Builder(),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
        EmptyLayout::Builder()
      ))
    );
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l.childAtIndex(0), LayoutCursor::Position::Right);
    c.performBackspace();
    assert_layout_serialize_to(l, "\u0014{\u0014}");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(1), LayoutCursor::Position::Right)));
  }
  /*
   * (1§▯)|23 -> BACKSPACE -> (1§|23]  ▯ == empty layout, § == prefix superscript
   */
  {
    Layout l = HorizontalLayout::Builder(
      ParenthesisLayout::Builder(HorizontalLayout::Builder(
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('1'),
          VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
        EmptyLayout::Builder()
      )),
      CodePointLayout::Builder('2'),
      CodePointLayout::Builder('3')
    );
    LayoutCursor c(l.childAtIndex(0), LayoutCursor::Position::Right);
    c.performBackspace();
    assert_layout_serialize_to(l, "(\u0014{1\u0014}23)");
    quiz_assert(c.isEquivalentTo(LayoutCursor(l.childAtIndex(0).childAtIndex(0).childAtIndex(1), LayoutCursor::Position::Left)));
  }
  /*
   * [1§▯)|^23 -> BACKSPACE -> 1§|▯^23  ▯ == empty layout, § == prefix superscript
   */
  {
    Layout l = HorizontalLayout::Builder(
      ParenthesisLayout::Builder(HorizontalLayout::Builder(
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('1'),
          VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
        EmptyLayout::Builder()
      )),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript),
      CodePointLayout::Builder('3')
    );
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l.childAtIndex(0), LayoutCursor::Position::Right);
    c.performBackspace();
    Layout l2 = HorizontalLayout::Builder(
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('1'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
      EmptyLayout::Builder(),
      VerticalOffsetLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript),
      CodePointLayout::Builder('3')
    );
    quiz_assert(l.isIdenticalTo(l2));
  }
}

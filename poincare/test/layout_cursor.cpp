#include <poincare/addition.h>
#include <poincare/cosine.h>
#include <poincare/empty_expression.h>
#include <poincare/integral.h>
#include <poincare/power.h>
#include <poincare/random.h>
#include <poincare_layouts.h>

#include "helper.h"

using namespace Poincare;

void assert_inserted_layout_points_to(Layout layoutToInsert,
                                      Expression correspondingExpression,
                                      Layout expectedLayoutAfterInsertion,
                                      int cursorPositionInLayout = 0) {
  HorizontalLayout h = HorizontalLayout::Builder();
  LayoutCursor c = LayoutCursor(h);
  bool insertingHorizontalLayout = layoutToInsert.isHorizontal();

  /* expectedLayoutAfterInsertion might be lost during insertion because it
   * could be a child of layoutToInsert.
   * So the address offset of its node is kept, and will be used after insertion
   * to check if the cursor is at the right layout.
   * It is also cloned so that it can be compared after insertion with an
   * isIdenticalTo.
   * */
  int addressOffsetBeforeInsertion =
      expectedLayoutAfterInsertion.node() - layoutToInsert.node();
  expectedLayoutAfterInsertion = expectedLayoutAfterInsertion.clone();

  /* LayoutField forces right of layout when expression has 0 children.
   * We mimic this behaviour here. */
  c.insertLayout(layoutToInsert, nullptr,
                 !correspondingExpression.isUninitialized() &&
                     correspondingExpression.numberOfChildren() == 0);

  int addressOffsetAfterInsertion =
      c.layout().node() - (insertingHorizontalLayout
                               ? static_cast<LayoutNode *>(h.node())
                               : h.childAtIndex(0).node());
  /* Check if the cursor layout is at the same adress offset and is identical
   * to the expected layout. */
  quiz_assert(addressOffsetAfterInsertion == addressOffsetBeforeInsertion &&
              expectedLayoutAfterInsertion.isIdenticalTo(c.layout()));
  quiz_assert(c.position() == cursorPositionInLayout);
}

QUIZ_CASE(poincare_layout_cursor_layout_to_point) {
  Layout l;
  Expression e;

  // 1+2
  l = HorizontalLayout::Builder(CodePointLayout::Builder('1'),
                                CodePointLayout::Builder('+'),
                                CodePointLayout::Builder('2'));
  e = Addition::Builder(Rational::Builder(1), Rational::Builder(2));
  assert_inserted_layout_points_to(l, e, l, 3);

  // random()
  l = HorizontalLayout::Builder(StringLayout::Builder("random"),
                                ParenthesisLayout::Builder());
  e = Random::Builder();
  assert_inserted_layout_points_to(l, e, l, 2);

  // cos(\x11)
  l = HorizontalLayout::Builder(StringLayout::Builder("cos"),
                                ParenthesisLayout::Builder());
  e = Cosine::Builder(EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, e, l.childAtIndex(1).childAtIndex(0));

  // ▯^▯
  l = VerticalOffsetLayout::Builder(
      HorizontalLayout::Builder(),
      VerticalOffsetLayoutNode::VerticalPosition::Superscript);
  e = Power::Builder(EmptyExpression::Builder(), EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, e, l.childAtIndex(0));

  // int(▯, x, ▯, ▯)
  l = IntegralLayout::Builder(
      HorizontalLayout::Builder(), CodePointLayout::Builder('x'),
      HorizontalLayout::Builder(), HorizontalLayout::Builder());
  e = Integral::Builder(EmptyExpression::Builder(), Symbol::Builder('x'),
                        EmptyExpression::Builder(), EmptyExpression::Builder());
  assert_inserted_layout_points_to(l, e, l.childAtIndex(2));
}

void assert_cursor_is_at(LayoutCursor cursor, Layout layout, int position) {
  quiz_assert(cursor.layout() == layout && cursor.position() == position);
}

QUIZ_CASE(poincare_layout_cursor_delete) {
  /*  12
   * --- -> "BackSpace" -> 12|34
   * |34
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(FractionLayout::Builder(
        LayoutHelper::StringToCodePointsLayout("12", 2),
        LayoutHelper::StringToCodePointsLayout("34", 2)));
    LayoutCursor cursor(layout.childAtIndex(0).childAtIndex(1),
                        OMG::Direction::Left());
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1234");
    assert_cursor_is_at(cursor, layout, 2);
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|3
   *     |3
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('1'), CodePointLayout::Builder('+'),
        FractionLayout::Builder(HorizontalLayout::Builder(),
                                CodePointLayout::Builder('3')));
    LayoutCursor cursor(layout.childAtIndex(2).childAtIndex(1),
                        OMG::Direction::Left());
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1+3");
    assert_cursor_is_at(cursor, layout, 2);
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|^2
   *     |^2
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('1'), CodePointLayout::Builder('+'),
        FractionLayout::Builder(
            HorizontalLayout::Builder(),
            VerticalOffsetLayout::Builder(
                CodePointLayout::Builder('2'),
                VerticalOffsetLayoutNode::VerticalPosition::Superscript)));
    LayoutCursor cursor(layout.childAtIndex(2).childAtIndex(1),
                        OMG::Direction::Left());
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1+^\u00122\u0013");
    assert_cursor_is_at(cursor, layout, 2);
  }

  /*
   * [[|1,▯][▯,▯]] -> "Backspace" -> |1
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        MatrixLayout::Builder(CodePointLayout::Builder('1')));
    LayoutCursor cursor(layout, OMG::Direction::Left());
    bool dummy;
    cursor.move(OMG::Direction::Right(), false, &dummy);
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "1");
    assert_cursor_is_at(cursor, layout, 0);
  }

  /* (I is the cursor in this comment since | is used for abs())
   * 2|I1|+3 -> "Backspace" -> 2I1+3
   * */
  {
    HorizontalLayout layout = HorizontalLayout::Builder(
        CodePointLayout::Builder('2'),
        AbsoluteValueLayout::Builder(CodePointLayout::Builder('1')),
        CodePointLayout::Builder('+'), CodePointLayout::Builder('3'));
    LayoutCursor cursor(layout.childAtIndex(1).childAtIndex(0),
                        OMG::Direction::Left());
    cursor.performBackspace();
    assert_layout_serialize_to(layout, "21+3");
    assert_cursor_is_at(cursor, layout, 1);
  }
}

QUIZ_CASE(poincare_layout_parentheses) {
  /*
   * |∅ -> "(" -> ")" -> ()|
   */
  {
    Layout l = HorizontalLayout::Builder();
    LayoutCursor c(l);
    c.insertText("(", nullptr);
    c.insertText(")", nullptr);
    assert_layout_serialize_to(l, "()");
    assert_cursor_is_at(c, l, l.numberOfChildren());
    quiz_assert(c.layout() == l && c.position() == l.numberOfChildren());
  }

  /*
   * |∅ -> ")" -> "(" -> ()(|)
   */
  {
    Layout l = HorizontalLayout::Builder();
    LayoutCursor c(l);
    c.insertText(")", nullptr);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "()()");
    assert_cursor_is_at(c, l.childAtIndex(1).childAtIndex(0), 0);
  }

  /*
   * |∅ -> "(" -> "(" -> ((|))
   */
  {
    Layout l = HorizontalLayout::Builder();
    LayoutCursor c(l);
    c.insertText("(", nullptr);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "(())");
    assert_cursor_is_at(
        c, l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0),
        0);
  }

  /*
   * 12|345 -> "(" -> 12(|345)
   */
  {
    Layout l = HorizontalLayout::Builder(
        {CodePointLayout::Builder('1'), CodePointLayout::Builder('2'),
         CodePointLayout::Builder('3'), CodePointLayout::Builder('4'),
         CodePointLayout::Builder('5')});
    LayoutCursor c(l);
    c.safeSetPosition(2);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "12(345)");
    assert_cursor_is_at(c, l.childAtIndex(2).childAtIndex(0), 0);
  }

  /*
   * 2|^3 -> "(" -> 2(|▯^3)  ▯ == empty rectangle
   */
  {
    Layout l = HorizontalLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayout::Builder(
            CodePointLayout::Builder('3'),
            VerticalOffsetLayoutNode::VerticalPosition::Superscript));
    LayoutCursor c(l);
    c.safeSetPosition(1);
    c.insertText("(", nullptr);
    assert_layout_serialize_to(l, "2(^\u00123\u0013)");
    assert_cursor_is_at(c, l.childAtIndex(1).childAtIndex(0), 0);
  }

  /*
   * (123)| -> BACKSPACE -> (123|)
   * */
  {
    Layout l = HorizontalLayout::Builder(
        ParenthesisLayout::Builder(HorizontalLayout::Builder(
            {CodePointLayout::Builder('1'), CodePointLayout::Builder('2'),
             CodePointLayout::Builder('3')})));
    LayoutCursor c(l, OMG::Direction::Right());
    c.performBackspace();
    assert_layout_serialize_to(l, "(123)");
    assert_cursor_is_at(c, l.childAtIndex(0).childAtIndex(0),
                        c.layout().numberOfChildren());
  }

  /*
   * (12(|34)5) -> BACKSPACE -> ((12|34)5)
   */
  {
    Layout l = HorizontalLayout::Builder(
        ParenthesisLayout::Builder(HorizontalLayout::Builder({
            CodePointLayout::Builder('1'),
            CodePointLayout::Builder('2'),
            ParenthesisLayout::Builder(HorizontalLayout::Builder(
                CodePointLayout::Builder('3'), CodePointLayout::Builder('4'))),
            CodePointLayout::Builder('5'),
        })));
    LayoutCursor c(
        l.childAtIndex(0).childAtIndex(0).childAtIndex(2).childAtIndex(0),
        OMG::Direction::Left());
    c.performBackspace();
    assert_layout_serialize_to(l, "((1234)5)");
    assert_cursor_is_at(
        c, l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0),
        2);
  }
  /*
   * ((|3)] -> BACKSPACE -> (|3)
   */
  {
    Layout l = HorizontalLayout::Builder(ParenthesisLayout::Builder(
        HorizontalLayout::Builder(ParenthesisLayout::Builder(
            HorizontalLayout::Builder(CodePointLayout::Builder('3'))))));
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())
        ->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, true);
    LayoutCursor c(
        l.childAtIndex(0).childAtIndex(0).childAtIndex(0).childAtIndex(0),
        OMG::Direction::Left());
    c.performBackspace();
    assert_layout_serialize_to(l, "(3)");
    assert_cursor_is_at(c, l.childAtIndex(0).childAtIndex(0), 0);
  }
  /*
   * sqrt((3]|) -> ")" -> sqrt((3)|)
   */
  {
    Layout l = HorizontalLayout::Builder(NthRootLayout::Builder(
        HorizontalLayout::Builder(ParenthesisLayout::Builder(
            HorizontalLayout::Builder(CodePointLayout::Builder('3'))))));
    static_cast<ParenthesisLayoutNode *>(
        l.childAtIndex(0).childAtIndex(0).childAtIndex(0).node())
        ->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, true);
    LayoutCursor c(l.childAtIndex(0).childAtIndex(0), OMG::Direction::Right());
    c.insertText(")", nullptr);
    assert_layout_serialize_to(l, "√\u0012(3)\u0013");
    assert_cursor_is_at(c, l.childAtIndex(0).childAtIndex(0), 1);
  }
  /*
   * |[} -> "{}" -> {|}{}
   */
  {
    Layout l = HorizontalLayout::Builder(CurlyBraceLayout::Builder());
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())
        ->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l, OMG::Direction::Left());
    c.insertLayout(CurlyBraceLayout::Builder(), nullptr);
    assert_layout_serialize_to(l, "{}{}");
    assert_cursor_is_at(c, l.childAtIndex(0).childAtIndex(0), 0);
  }
  /*
   * [▯§▯)| -> BACKSPACE -> ▯§▯|  ▯ == empty rectangle, § == prefix superscript
   */
  {
    Layout l = HorizontalLayout::Builder(ParenthesisLayout::Builder(
        HorizontalLayout::Builder(VerticalOffsetLayout::Builder(
            HorizontalLayout::Builder(),
            VerticalOffsetLayoutNode::VerticalPosition::Superscript,
            VerticalOffsetLayoutNode::HorizontalPosition::Prefix))));
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())
        ->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l, OMG::Direction::Right());
    c.performBackspace();
    assert_layout_serialize_to(l, "\u0014{\u0014}");
    assert_cursor_is_at(c, l, 1);
  }
  /*
   * (1§▯)|23 -> BACKSPACE -> (1§|23]  ▯ == empty rectangle, § == prefix
   * superscript
   */
  {
    Layout l = HorizontalLayout::Builder(
        ParenthesisLayout::Builder(
            HorizontalLayout::Builder(VerticalOffsetLayout::Builder(
                CodePointLayout::Builder('1'),
                VerticalOffsetLayoutNode::VerticalPosition::Superscript,
                VerticalOffsetLayoutNode::HorizontalPosition::Prefix))),
        CodePointLayout::Builder('2'), CodePointLayout::Builder('3'));
    LayoutCursor c(l);
    c.safeSetPosition(1);
    c.performBackspace();
    assert_layout_serialize_to(l, "(\u0014{1\u0014}23)");
    assert_cursor_is_at(c, l.childAtIndex(0).childAtIndex(0), 1);
  }
  /*
   * [1§▯)|^23 -> BACKSPACE -> 1§|▯^23  ▯ == empty rectangle, § == prefix
   * superscript
   */
  {
    Layout l = HorizontalLayout::Builder(
        ParenthesisLayout::Builder(
            HorizontalLayout::Builder(VerticalOffsetLayout::Builder(
                CodePointLayout::Builder('1'),
                VerticalOffsetLayoutNode::VerticalPosition::Superscript,
                VerticalOffsetLayoutNode::HorizontalPosition::Prefix))),
        VerticalOffsetLayout::Builder(
            CodePointLayout::Builder('2'),
            VerticalOffsetLayoutNode::VerticalPosition::Superscript),
        CodePointLayout::Builder('3'));
    static_cast<ParenthesisLayoutNode *>(l.childAtIndex(0).node())
        ->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    LayoutCursor c(l);
    c.safeSetPosition(1);
    c.performBackspace();
    Layout l2 = HorizontalLayout::Builder(
        VerticalOffsetLayout::Builder(
            CodePointLayout::Builder('1'),
            VerticalOffsetLayoutNode::VerticalPosition::Superscript,
            VerticalOffsetLayoutNode::HorizontalPosition::Prefix),
        VerticalOffsetLayout::Builder(
            CodePointLayout::Builder('2'),
            VerticalOffsetLayoutNode::VerticalPosition::Superscript),
        CodePointLayout::Builder('3'));
    quiz_assert(l.isIdenticalTo(l2));
    assert_cursor_is_at(c, l, 1);
  }
}

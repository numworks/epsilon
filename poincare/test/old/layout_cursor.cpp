#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layout_cursor.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal::KTrees;

void assert_cursor_layout_is(Poincare::Internal::LayoutBufferCursor c,
                             const Poincare::Internal::Tree* rootRack) {
  quiz_assert(c.rootRack()->treeIsIdenticalTo(rootRack));
}

void assert_cursor_position_is(Poincare::Internal::LayoutBufferCursor c,
                               Poincare::Internal::Tree* cursorRack,
                               int position) {
  quiz_assert(c.cursorRack() == cursorRack);
  quiz_assert(c.position() == position);
}

void assert_cursor_is(Poincare::Internal::LayoutBufferCursor c,
                      const Poincare::Internal::Tree* rootRack,
                      Poincare::Internal::Tree* cursorRack, int position) {
  assert_cursor_layout_is(c, rootRack);
  assert_cursor_position_is(c, cursorRack, position);
}

Poincare::Internal::LayoutBufferCursor insert_layout(
    const Poincare::Internal::Tree* layout) {
  Layout r = KRackL();
  Poincare::Internal::LayoutBufferCursor c(r, r.tree());
  c.insertLayout(layout);
  assert_cursor_layout_is(c, layout);
  return c;
}

QUIZ_CASE(poincare_layout_cursor_insertion) {
  // 1+2
  {
    Poincare::Internal::LayoutBufferCursor c = insert_layout("1+2"_l);
    assert_cursor_position_is(c, c.rootRack(), 3);
  }

  // random()
  {
    Poincare::Internal::LayoutBufferCursor c = insert_layout("random()"_l);
    assert_cursor_position_is(c, c.rootRack(), 8);
  }

  // cos()
  {
    Poincare::Internal::LayoutBufferCursor c = insert_layout("cos()"_l);
#if 0  // TODO_PCJ:
    assert_cursor_position_is(c, c.rootRack()->child(0)->child(0), 0);
#endif
  }

  // ▯^▯
  {
    Poincare::Internal::LayoutBufferCursor c =
        insert_layout(KRackL(KSuperscriptL(KRackL())));
    assert_cursor_position_is(c, c.rootRack()->child(0)->child(0), 0);
  }

  // int(▯, x, ▯, ▯)
  {
    Poincare::Internal::LayoutBufferCursor c =
        insert_layout(KRackL(KIntegralL("x"_l, KRackL(), KRackL(), KRackL())));
    assert_cursor_position_is(c, c.rootRack()->child(0)->child(1), 0);
  }
}

QUIZ_CASE(poincare_layout_cursor_deletion) {
  /*  12
   * --- -> "BackSpace" -> 12|34
   * |34
   * */
  {
    Layout l = KRackL(KFracL("12"_l, "34"_l));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree()->child(0)->child(1),
                                             OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(c, "1234"_l, c.rootRack(), 2);
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|3
   *     |3
   * */
  {
    Layout l = "1+"_l ^ KFracL(KRackL(), "3"_l);
    Poincare::Internal::LayoutBufferCursor c(l, l.tree()->child(2)->child(1),
                                             OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(c, "1+3"_l, c.rootRack(), 2);
  }

  /*      ▯
   * 1 + --- -> "BackSpace" -> 1+|^2
   *     |^2
   * */
  {
    Layout l = "1+"_l ^ KFracL(KRackL(), KRackL(KSuperscriptL("2"_l)));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree()->child(2)->child(1),
                                             OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(c, "1+"_l ^ KSuperscriptL("2"_l), c.rootRack(), 2);
  }

  /*
   * [[|1,▯][▯,▯]] -> "Backspace" -> |1
   * */
  {
    Layout l = KRackL(KMatrix1x1L("1"_l));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree(),
                                             OMG::Direction::Left());
    bool dummy;
    c.move(OMG::Direction::Right(), false, &dummy);
    c.performBackspace();
    assert_cursor_is(c, "1"_l, c.rootRack(), 0);
  }

  /* (I is the cursor in this comment since | is used for abs())
   * 2|I1|+3 -> "Backspace" -> 2I1+3
   * */
  {
    Layout l = "2"_l ^ KAbsL("1"_l) ^ "+3"_l;
    Poincare::Internal::LayoutBufferCursor c(l, l.tree()->child(1)->child(0),
                                             OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(c, "21+3"_l, c.rootRack(), 1);
  }
}

QUIZ_CASE(poincare_layout_parentheses) {
  /*
   * |∅ -> "(" -> ")" -> ()|
   */
  {
    Layout l = KRackL();
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.insertText("(");
    c.insertText(")");
    assert_cursor_is(c, KRackL(KParenthesesL(KRackL())), c.rootRack(),
                     c.rootRack()->numberOfChildren());
  }

  /*
   * |∅ -> ")" -> "(" -> ()(|)
   */
  {
    Layout l = KRackL();
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.insertText(")");
    c.insertText("(");
    assert_cursor_is(
        c, KParenthesesLeftTempL(KRackL()) ^ KParenthesesRightTempL(KRackL()),
        c.rootRack()->child(1)->child(0), 0);
  }

  /*
   * |∅ -> "(" -> "(" -> ((|))
   */
  {
    Layout l = KRackL();
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.insertText("(");
    c.insertText("(");
    assert_cursor_is(c,
                     KRackL(KParenthesesRightTempL(
                         KRackL(KParenthesesRightTempL(KRackL())))),
                     c.rootRack()->child(0)->child(0)->child(0)->child(0), 0);
  }

  /*
   * 12|345 -> "(" -> 12(|345)
   */
  {
    Layout l = "12345"_l;
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.safeSetPosition(2);
    c.insertText("(");
    assert_cursor_is(c, "12"_l ^ KParenthesesRightTempL("345"_l),
                     c.rootRack()->child(2)->child(0), 0);
  }

  /*
   * 2|^3 -> "(" -> 2(|▯^3)  ▯ == empty rectangle
   */
  {
    Layout l = "2"_l ^ KSuperscriptL("3"_l);
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.safeSetPosition(1);
    c.insertText("(");
    assert_cursor_is(
        c, "2"_l ^ KParenthesesRightTempL(KRackL(KSuperscriptL("3"_l))),
        c.rootRack()->child(1)->child(0), 0);
  }

  /*
   * (123)| -> BACKSPACE -> (123|)
   * */
  {
    Layout l = KRackL(KParenthesesL("123"_l));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.performBackspace();
    assert_cursor_is(c, KRackL(KParenthesesRightTempL("123"_l)),
                     c.rootRack()->child(0)->child(0),
                     c.cursorRack()->numberOfChildren());
  }

  /*
   * (12(|34)5) -> BACKSPACE -> ((12|34)5)
   */
  {
    Layout l = KRackL(KParenthesesL("12"_l ^ KParenthesesL("34"_l) ^ "5"_l));
    Poincare::Internal::LayoutBufferCursor c(
        l, l.tree()->child(0)->child(0)->child(2)->child(0),
        OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(
        c, KRackL(KParenthesesLeftTempL(KParenthesesL("1234"_l) ^ "5"_l)),
        c.rootRack()->child(0)->child(0)->child(0)->child(0), 2);
  }

  /*
   * ((|3)] -> BACKSPACE -> (|3)
   */
  {
    Layout l = KRackL(KParenthesesRightTempL(KRackL(KParenthesesL("3"_l))));
    Poincare::Internal::LayoutBufferCursor c(
        l, l.tree()->child(0)->child(0)->child(0)->child(0),
        OMG::Direction::Left());
    c.performBackspace();
    assert_cursor_is(c, KRackL(KParenthesesL("3"_l)),
                     c.rootRack()->child(0)->child(0), 0);
  }

  /*
   * sqrt((3]|) -> ")" -> sqrt((3)|)
   */
  {
    Layout l = KRackL(KRootL(KRackL(KParenthesesRightTempL("3"_l)), "2"_l));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree()->child(0)->child(0));
    c.insertText(")");
    assert_cursor_is(c, KRackL(KRootL(KRackL(KParenthesesL("3"_l)), "2"_l)),
                     c.rootRack()->child(0)->child(0), 1);
  }

  /*
   * |[} -> "{}" -> {|}{}
   */
  {
    Layout l = KRackL(KCurlyBracesLeftTempL(KRackL()));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree(),
                                             OMG::Direction::Left());
    c.insertLayout(KCurlyBracesL(KRackL()));
#if 0  // TODO_PCJ
    assert_cursor_is(c, KCurlyBracesL(KRackL()) ^ KCurlyBracesL(KRackL()),
                     c.rootRack()->child(0)->child(0), 0);
#endif
  }

  /*
   * [▯§▯)| -> BACKSPACE -> ▯§▯|  ▯ == empty rectangle, § == prefix superscript
   */
  {
    Layout l =
        KRackL(KParenthesesLeftTempL(KRackL(KPrefixSuperscriptL(KRackL()))));
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.performBackspace();
    assert_cursor_is(c, KRackL(KPrefixSuperscriptL(KRackL())), c.rootRack(), 1);
  }

  /*
   * (1§▯)|23 -> BACKSPACE -> (1§|23]  ▯ == empty rectangle, § == prefix
   * superscript
   */
  {
    Layout l = KParenthesesL(KRackL(KPrefixSuperscriptL("1"_l))) ^ "23"_l;
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.safeSetPosition(1);
    c.performBackspace();
    assert_cursor_is(
        c, KRackL(KParenthesesRightTempL(KPrefixSuperscriptL("1"_l) ^ "23"_l)),
        c.rootRack()->child(0)->child(0), 1);
  }

  /*
   * [1§▯)|^23 -> BACKSPACE -> 1§|▯^23  ▯ == empty rectangle, § == prefix
   * superscript
   */
  {
    Layout l = KParenthesesLeftTempL(KRackL(KPrefixSuperscriptL("1"_l))) ^
               KSuperscriptL("2"_l) ^ "3"_l;
    Poincare::Internal::LayoutBufferCursor c(l, l.tree());
    c.safeSetPosition(1);
    c.performBackspace();
    assert_cursor_is(c,
                     KPrefixSuperscriptL("1"_l) ^ KSuperscriptL("2"_l) ^ "3"_l,
                     c.rootRack(), 1);
  }
}

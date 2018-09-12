#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_fraction_layout_create) {

  /*                         12
   * 12|34+5 -> "Divide" -> --- + 5
   *                        |34
   * */
  HorizontalLayoutRef layout = static_cast<HorizontalLayoutRef>(LayoutHelper::String("1234+5", 6));
  LayoutCursor cursor(layout.childAtIndex(2), LayoutCursor::Position::Left);
  cursor.addFractionLayoutAndCollapseSiblings();
  assert_expression_layout_serialize_to(layout, "(12)/(34)+5");
  quiz_assert(cursor.isEquivalentTo(LayoutCursor(layout.childAtIndex(0).childAtIndex(1), LayoutCursor::Position::Left)));
}

QUIZ_CASE(poincare_fraction_layout_delete) {

  /*  12
   * --- -> "BackSpace" -> 12|34
   * |34
   * */
  HorizontalLayoutRef layout1 = HorizontalLayoutRef(
      FractionLayoutRef(
        LayoutHelper::String("12", 2),
        LayoutHelper::String("34", 2)
      )
    );
  LayoutCursor cursor1(layout1.childAtIndex(0).childAtIndex(1), LayoutCursor::Position::Left);
  cursor1.performBackspace();
  assert_expression_layout_serialize_to(layout1, "1234");
  quiz_assert(cursor1.isEquivalentTo(LayoutCursor(layout1.childAtIndex(1), LayoutCursor::Position::Right)));

  /*      ø
   * 1 + --- -> "BackSpace" -> 1+|3
   *     |3
   * */
  HorizontalLayoutRef layout2 = HorizontalLayoutRef(
      CharLayoutRef('1'),
      CharLayoutRef('+'),
      FractionLayoutRef(
        EmptyLayoutRef(),
        CharLayoutRef('3')
      )
    );
  LayoutCursor cursor2(layout2.childAtIndex(2).childAtIndex(1), LayoutCursor::Position::Left);
  cursor2.performBackspace();
  assert_expression_layout_serialize_to(layout2, "1+3");
  quiz_assert(cursor2.isEquivalentTo(LayoutCursor(layout2.childAtIndex(1), LayoutCursor::Position::Right)));
}

QUIZ_CASE(poincare_fraction_layout_serialize) {
  FractionLayoutRef layout = FractionLayoutRef(
      CharLayoutRef('1'),
      LayoutHelper::String("2+3", 3)
    );
  assert_expression_layout_serialize_to(layout, "(1)/(2+3)");
}

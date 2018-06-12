#include <quiz.h>
#include <poincare.h>
#include <poincare_layouts.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_fraction_layout_create) {

  /*                         12
   * 12|34+5 -> "Divide" -> --- + 5
   *                        |34
   * */
  HorizontalLayout * layout = static_cast<HorizontalLayout *>(LayoutEngine::createStringLayout("1234+5", 6));
  ExpressionLayoutCursor cursor(layout->editableChild(2), ExpressionLayoutCursor::Position::Left);
  cursor.addFractionLayoutAndCollapseSiblings();
  assert_expression_layout_serialize_to(layout, "(12)/(34)+5");
  assert(cursor.isEquivalentTo(ExpressionLayoutCursor(layout->editableChild(0)->editableChild(1), ExpressionLayoutCursor::Position::Left)));
  delete layout;
}

QUIZ_CASE(poincare_fraction_layout_delete) {

  /*  12
   * --- -> "BackSpace" -> 12|34
   * |34
   * */
  HorizontalLayout * layout1 = new HorizontalLayout(
      new FractionLayout(
        LayoutEngine::createStringLayout("12", 2),
        LayoutEngine::createStringLayout("34", 2),
        false),
      false);
  ExpressionLayoutCursor cursor1(layout1->editableChild(0)->editableChild(1), ExpressionLayoutCursor::Position::Left);
  cursor1.performBackspace();
  assert_expression_layout_serialize_to(layout1, "1234");
  assert(cursor1.isEquivalentTo(ExpressionLayoutCursor(layout1->editableChild(1), ExpressionLayoutCursor::Position::Right)));
  delete layout1;

  /*      •
   * 1 + --- -> "BackSpace" -> 1+|3
   *     |3
   * */
  HorizontalLayout * layout2 = new HorizontalLayout(
      new CharLayout('1'),
      new CharLayout('+'),
      new FractionLayout(
        new EmptyLayout(),
        new CharLayout('3'),
        false),
      false);
  ExpressionLayoutCursor cursor2(layout2->editableChild(2)->editableChild(1), ExpressionLayoutCursor::Position::Left);
  cursor2.performBackspace();
  assert_expression_layout_serialize_to(layout2, "1+3");
  assert(cursor2.isEquivalentTo(ExpressionLayoutCursor(layout2->editableChild(1), ExpressionLayoutCursor::Position::Right)));
  delete layout2;
}

QUIZ_CASE(poincare_fraction_layout_serialize) {
  FractionLayout * layout = new FractionLayout(
      new CharLayout('1'),
      LayoutEngine::createStringLayout("2+3", 3),
      false);
  assert_expression_layout_serialize_to(layout, "(1)/(2+3)");
  delete layout;
}

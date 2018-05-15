#include <quiz.h>
#include <poincare.h>
#include <poincare_layouts.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_vertical_offset_layout_serialize) {
  assert_parsed_expression_layout_serialize_to_self("2^3");

  HorizontalLayout * layout = new HorizontalLayout(
      new CharLayout('2'),
      new VerticalOffsetLayout(
        LayoutEngine::createStringLayout("4+5", 3),
        VerticalOffsetLayout::Type::Superscript,
        false),
      false);
  assert_expression_layout_serialize_to(layout, "2^(4+5)");
  delete layout;

}

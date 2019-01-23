#include <quiz.h>
#include <poincare_layouts.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_vertical_offset_layout_serialize) {
  HorizontalLayout layout = HorizontalLayout::Builder(
    CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        LayoutHelper::String("x+5", 3),
        VerticalOffsetLayoutNode::Type::Superscript
      )
    );
  assert(UCodePointLeftSuperscript == '\x12');
  assert(UCodePointRightSuperscript == '\x13');
  assert_expression_layout_serialize_to(layout, "2\x12x+5\x13");
}

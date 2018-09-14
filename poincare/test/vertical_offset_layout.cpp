#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_vertical_offset_layout_serialize) {
  assert_parsed_expression_layout_serialize_to_self("(2)^(3)");

  HorizontalLayoutReference layout = HorizontalLayoutReference(
    CharLayoutReference('2'),
      VerticalOffsetLayoutReference(
        LayoutHelper::String("4+5", 3),
        VerticalOffsetLayoutNode::Type::Superscript
      )
    );
  assert_expression_layout_serialize_to(layout, "2^(4+5)");
}

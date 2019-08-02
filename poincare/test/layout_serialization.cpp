#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <poincare_layouts.h>
#include "helper.h"

using namespace Poincare;

// TODO: add tests about layout serialization (especially all serialization that add system parentheses)

QUIZ_CASE(poincare_layout_serialization) {
  // Binomial coefficient layout
  assert_layout_serialize_to(BinomialCoefficientLayout::Builder(CodePointLayout::Builder('7'), CodePointLayout::Builder('6')), "binomial\u00127,6\u0013");

  // Fraction layout
  Layout layout = FractionLayout::Builder(
      CodePointLayout::Builder('1'),
      LayoutHelper::String("2+3", 3)
    );
  assert_layout_serialize_to(layout, "\u0012\u00121\u0013/\u00122+3\u0013\u0013"); // 1/(2+3)

  // Nth root layout
  assert_layout_serialize_to(NthRootLayout::Builder(CodePointLayout::Builder('7'), CodePointLayout::Builder('6')), "root\u00127,6\u0013");

  // Vertical offset layout
  layout = HorizontalLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayout::Builder(
        LayoutHelper::String("x+5", 3),
        VerticalOffsetLayoutNode::Position::Superscript
        )
      );
  assert_layout_serialize_to(layout, "2^\x12x+5\x13");
}

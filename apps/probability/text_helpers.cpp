#include "text_helpers.h"

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <stdarg.h>
#include <string.h>

#include "poincare/preferences.h"
#include "shared/poincare_helpers.h"

using namespace Poincare;

namespace Probability {

int defaultConvertFloatToText(double value, char buffer[], int bufferSize) {
  return Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      value,
      buffer,
      bufferSize,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      Poincare::Preferences::PrintFloatMode::Decimal);
}

Poincare::Layout XOneMinusXTwoLayout() {
  HorizontalLayout x1 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout x2 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout res = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(x2, 1, true);
  res.addOrMergeChildAtIndex(x1, 0, true);
  return std::move(res);
}

Poincare::HorizontalLayout codePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript) {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(base),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(subscript, KDFont::LargeFont),
                                    VerticalOffsetLayoutNode::Position::Subscript));
}

Poincare::Layout setSmallFont(Poincare::Layout layout) {
  if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout) {
    static_cast<Poincare::CodePointLayout *>(&layout)->setFont(KDFont::SmallFont);
  }
  for (int i = 0; i < layout.numberOfChildren(); i++) {
    setSmallFont(layout.childAtIndex(i));
  }
  return layout;
}

}  // namespace Probability

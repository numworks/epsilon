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

using namespace Poincare;

namespace Inference {

Poincare::Layout XOneMinusXTwoLayout() {
  HorizontalLayout x1 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  HorizontalLayout x2 = HorizontalLayout::Builder(
      CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  HorizontalLayout res = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(x2, 1, true);
  res.addOrMergeChildAtIndex(x1, 0, true);
  return std::move(res);
}

}  // namespace Inference

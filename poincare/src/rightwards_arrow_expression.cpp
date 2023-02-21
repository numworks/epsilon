#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/rightwards_arrow_expression.h>
#include <poincare/serialization_helper.h>

#include <utility>

namespace Poincare {

int RightwardsArrowExpressionNode::serialize(
    char* buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize,
                                 UCodePointRightwardsArrow);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, string);
}

Layout RightwardsArrowExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context),
      0);
  result.addChildAtIndexInPlace(
      CodePointLayout::Builder(UCodePointRightwardsArrow),
      result.numberOfChildren(), result.numberOfChildren());
  result.addOrMergeChildAtIndex(
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits,
                                    context),
      result.numberOfChildren());
  return std::move(result);
}

}  // namespace Poincare

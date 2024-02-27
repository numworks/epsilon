#include <ion/unicode/utf8_decoder.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/rightwards_arrow_expression.h>
#include <poincare/serialization_helper.h>

#include <utility>

namespace Poincare {

size_t RightwardsArrowExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  constexpr size_t stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize,
                                 UCodePointRightwardsArrow);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, string);
}

OLayout RightwardsArrowExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  constexpr CodePoint arrow = UCodePointRightwardsArrow;
  constexpr size_t k_sizeOfArrow = UTF8Decoder::CharSizeOfCodePoint(arrow) + 1;
  char arrowString[k_sizeOfArrow];
  SerializationHelper::CodePoint(arrowString, k_sizeOfArrow, arrow);
  return LayoutHelper::Infix(Expression(this), floatDisplayMode,
                             numberOfSignificantDigits, arrowString, context);
}

}  // namespace Poincare

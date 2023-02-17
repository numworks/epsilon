#include <poincare/list_sequence_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/list_sequence.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <algorithm>

namespace Poincare {

int ListSequenceLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSequence::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

int ListSequenceLayoutNode::indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) {
  switch (currentIndex) {
  case k_outsideIndex:
    return direction == OMG::HorizontalDirection::Right ? k_functionLayoutIndex : k_upperBoundLayoutIndex;
  case k_functionLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_variableLayoutIndex : k_outsideIndex;
  case k_variableLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_upperBoundLayoutIndex : k_functionLayoutIndex;
  default:
    assert(currentIndex == k_upperBoundLayoutIndex);
    return direction == OMG::HorizontalDirection::Right ? k_outsideIndex : k_variableLayoutIndex;
  }
}

LayoutNode::DeletionMethod ListSequenceLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  return StandardDeletionMethodForLayoutContainingArgument(childIndex, k_functionLayoutIndex);
}

Layout ListSequenceLayoutNode::XNTLayout(int childIndex) const {
  if (childIndex == k_functionLayoutIndex) {
    return Layout(childAtIndex(k_variableLayoutIndex)).clone();
  }
  if (childIndex == k_variableLayoutIndex) {
    return CodePointLayout::Builder(CodePoint(ListSequence::k_defaultXNTChar));
  }
  return LayoutNode::XNTLayout();
}

KDSize ListSequenceLayoutNode::computeSize(KDFont::Size font) {
  KDPoint upperBoundPosition = positionOfChild(upperBoundLayout(), font);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  return KDSize(
      upperBoundPosition.x() + upperBoundSize.width(),
      std::max(upperBoundPosition.y() + upperBoundSize.height(), positionOfVariable(font).y() + variableLayout()->layoutSize(font).height()));
}

KDPoint ListSequenceLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  if (child == variableLayout()) {
    return positionOfVariable(font);
  }
  if (child == functionLayout()) {
    return KDPoint(CurlyBraceLayoutNode::k_curlyBraceWidth, baseline(font) - functionLayout()->baseline(font));
  }
  assert(child == upperBoundLayout());
  return KDPoint(
      positionOfVariable(font).x() + variableLayout()->layoutSize(font).width() + KDFont::Font(font)->stringSize("≤").width(),
      variableSlotBaseline(font) - upperBoundLayout()->baseline(font));
}

KDPoint ListSequenceLayoutNode::positionOfVariable(KDFont::Size font) {
  return KDPoint(
     k_variableHorizontalMargin + bracesWidth(font),
     variableSlotBaseline(font) - variableLayout()->baseline(font));
}

KDCoordinate ListSequenceLayoutNode::variableSlotBaseline(KDFont::Size font) {
  return std::max({static_cast<int>(CurlyBraceLayoutNode::HeightGivenChildHeight(functionLayout()->layoutSize(font).height()) + k_variableBaselineOffset),
                  static_cast<int>(upperBoundLayout()->baseline(font)),
                  static_cast<int>(variableLayout()->baseline(font))});
}

KDCoordinate ListSequenceLayoutNode::computeBaseline(KDFont::Size font) {
  return CurlyBraceLayoutNode::BaselineGivenChildHeightAndBaseline(functionLayout()->layoutSize(font).height(), functionLayout()->baseline(font));
}

KDCoordinate ListSequenceLayoutNode::bracesWidth(KDFont::Size font) {
  return 2 * CurlyBraceLayoutNode::k_curlyBraceWidth + functionLayout()->layoutSize(font).width();
}

void ListSequenceLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  // Draw {  }
  KDSize functionSize = functionLayout()->layoutSize(font);
  KDPoint functionPosition = positionOfChild(functionLayout(), font);
  KDCoordinate functionBaseline = functionLayout()->baseline(font);

  KDPoint leftBracePosition = CurlyBraceLayoutNode::PositionGivenChildHeightAndBaseline(true, functionSize, functionBaseline).translatedBy(functionPosition);
  CurlyBraceLayoutNode::RenderWithChildHeight(true, functionSize.height(), ctx, leftBracePosition.translatedBy(p), expressionColor, backgroundColor);

  KDPoint rightBracePosition = CurlyBraceLayoutNode::PositionGivenChildHeightAndBaseline(false, functionSize, functionBaseline).translatedBy(functionPosition);
  CurlyBraceLayoutNode::RenderWithChildHeight(false, functionSize.height(), ctx, rightBracePosition.translatedBy(p), expressionColor, backgroundColor);

  // Draw k≤...
  KDPoint inferiorEqualPosition = KDPoint(positionOfVariable(font).x() + variableLayout()->layoutSize(font).width(), variableSlotBaseline(font) - KDFont::GlyphHeight(font) / 2);
  ctx->drawString("≤", inferiorEqualPosition.translatedBy(p), font, expressionColor, backgroundColor);
}

}

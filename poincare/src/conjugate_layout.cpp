#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <assert.h>

namespace Poincare {
/*
void ConjugateLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (!deleteBeforeCursorForLayoutContainingArgument(childLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}*/

int ConjugateLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

KDSize ConjugateLayoutNode::computeSize(KDFont::Size font) {
  KDSize childSize = childLayout()->layoutSize(font);
  KDCoordinate newWidth =
    Escher::Metric::FractionAndConjugateHorizontalMargin +
    Escher::Metric::FractionAndConjugateHorizontalOverflow +
    childSize.width() +
    Escher::Metric::FractionAndConjugateHorizontalOverflow +
    Escher::Metric::FractionAndConjugateHorizontalMargin;
  KDCoordinate newHeight =
    childSize.height() +
    k_overlineWidth +
    k_overlineVerticalMargin;
  return KDSize(newWidth, newHeight);
}

KDCoordinate ConjugateLayoutNode::computeBaseline(KDFont::Size font) {
  return childLayout()->baseline(font) + k_overlineWidth + k_overlineVerticalMargin;
}

KDPoint ConjugateLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(child == childLayout());
  return KDPoint(
      Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow,
      k_overlineWidth + k_overlineVerticalMargin);
}

void ConjugateLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(
      KDRect(
        p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin,
        p.y(),
        childLayout()->layoutSize(font).width() + 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow,
        k_overlineWidth),
      expressionColor);
}

}

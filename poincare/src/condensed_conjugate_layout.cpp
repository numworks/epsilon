#include <escher/metric.h>
#include <poincare/condensed_conjugate_layout.h>

namespace Poincare {

KDSize CondensedConjugateLayoutNode::computeSize() {
  KDSize childSize = childLayout()->layoutSize();
  KDCoordinate newHeight = childSize.height() + k_overlineWidth + k_overlineVerticalMargin;
  KDCoordinate newWidth = childSize.width() +
                          2 * Escher::Metric::FractionAndConjugateHorizontalMargin;
  return KDSize(newWidth, newHeight);
}

KDPoint CondensedConjugateLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == childLayout());
  return KDPoint(Escher::Metric::FractionAndConjugateHorizontalMargin,
                 k_overlineWidth + k_overlineVerticalMargin);
}

void CondensedConjugateLayoutNode::render(KDContext * ctx,
                                          KDPoint p,
                                          KDColor expressionColor,
                                          KDColor backgroundColor,
                                          Layout * selectionStart,
                                          Layout * selectionEnd,
                                          KDColor selectionColor) {
  ctx->fillRect(KDRect(p,
                       childLayout()->layoutSize().width() +
                           2 * Escher::Metric::FractionAndConjugateHorizontalMargin,
                       k_overlineWidth),
                expressionColor);
}

}  // namespace Poincare

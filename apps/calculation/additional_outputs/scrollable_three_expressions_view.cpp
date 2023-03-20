#include "scrollable_three_expressions_view.h"

#include <poincare/exception_checkpoint.h>

#include "../app.h"

using namespace Escher;

namespace Calculation {

void ScrollableThreeExpressionsView::ContentCell::reloadTextColor() {
  KDColor color = displayCenter() && !rightIsStrictlyEqual()
                      ? Palette::GrayVeryDark
                      : KDColorBlack;
  rightExpressionView()->setTextColor(color);
  approximateSign()->setTextColor(color);
}

void ScrollableThreeExpressionsView::resetMemoization() {
  setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());
}

void ScrollableThreeExpressionsView::setLayouts(Poincare::Layout leftLayout,
                                                Poincare::Layout centerLayout,
                                                Poincare::Layout rightLayout) {
  Shared::AbstractScrollableMultipleExpressionsView::setLayouts(
      leftLayout, centerLayout, rightLayout);
  setShowEqual(!centerLayout.isUninitialized() ||
               !rightLayout.isUninitialized());
}

ScrollableThreeExpressionsView::SubviewPosition
ScrollableThreeExpressionsView::leftMostPosition() {
  if (!m_contentCell.m_leftExpressionView.layout().isUninitialized()) {
    return SubviewPosition::Left;
  } else if (displayCenter()) {
    return SubviewPosition::Center;
  }
  return SubviewPosition::Right;
}

}  // namespace Calculation

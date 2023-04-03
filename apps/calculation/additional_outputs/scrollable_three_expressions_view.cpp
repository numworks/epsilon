#include "scrollable_three_expressions_view.h"

#include <poincare/exception_checkpoint.h>

#include "../app.h"

using namespace Escher;
using namespace Poincare;

namespace Calculation {

void ScrollableThreeExpressionsView::ContentCell::reloadTextColor() {
  KDColor color = displayCenter() && !rightIsStrictlyEqual()
                      ? Palette::GrayVeryDark
                      : KDColorBlack;
  rightExpressionView()->setTextColor(color);
  approximateSign()->setTextColor(color);
}

void ScrollableThreeExpressionsView::setLayouts(Layout formulaLayout,
                                                Layout exactLayout,
                                                Layout approximateLayout) {
  Shared::AbstractScrollableMultipleExpressionsView::setLayouts(
      formulaLayout, exactLayout, approximateLayout);
  setShowEqualSignAfterFormula(!exactLayout.isUninitialized() ||
                               !approximateLayout.isUninitialized());
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

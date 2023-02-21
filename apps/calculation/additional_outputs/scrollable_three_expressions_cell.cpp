#include "scrollable_three_expressions_cell.h"

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

ScrollableThreeExpressionsView::SubviewPosition
ScrollableThreeExpressionsView::leftMostPosition() {
  if (!m_contentCell.m_leftExpressionView.layout().isUninitialized()) {
    return SubviewPosition::Left;
  } else if (displayCenter()) {
    return SubviewPosition::Center;
  }
  return SubviewPosition::Right;
}

void ScrollableThreeExpressionsCell::setLayouts(Poincare::Layout leftLayout,
                                                Poincare::Layout centerLayout,
                                                Poincare::Layout rightLayout) {
  m_view.setLayouts(leftLayout, centerLayout, rightLayout);
  m_view.setShowEqual(!centerLayout.isUninitialized() ||
                      !rightLayout.isUninitialized());
}

void ScrollableThreeExpressionsCell::didBecomeFirstResponder() {
  reinitSelection();
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableThreeExpressionsCell::reinitSelection() {
  m_view.setSelectedSubviewPosition(m_view.leftMostPosition());
  m_view.reloadScroll();
}

}  // namespace Calculation

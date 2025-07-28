#include "scrollable_three_layouts_view.h"

using namespace Escher;
using namespace Poincare;

namespace Calculation {

void ScrollableThreeLayoutsView::ContentCell::reloadTextColor() {
  KDColor color = displayCenter() && !rightIsStrictlyEqual()
                      ? Palette::GrayVeryDark
                      : KDColorBlack;
  rightLayoutView()->setTextColor(color);
  approximateSign()->setTextColor(color);
}

void ScrollableThreeLayoutsView::setLayouts(Layout formulaLayout,
                                            Layout exactLayout,
                                            Layout approximateLayout) {
  AbstractScrollableMultipleLayoutsView::setLayouts(formulaLayout, exactLayout,
                                                    approximateLayout);
  setShowEqualSignAfterFormula(!exactLayout.isUninitialized() ||
                               !approximateLayout.isUninitialized());
}

ScrollableThreeLayoutsView::SubviewPosition
ScrollableThreeLayoutsView::leftMostPosition() {
  if (!m_contentCell.m_leftLayoutView.layout().isUninitialized()) {
    return SubviewPosition::Left;
  } else if (displayCenter()) {
    return SubviewPosition::Center;
  }
  return SubviewPosition::Right;
}

}  // namespace Calculation

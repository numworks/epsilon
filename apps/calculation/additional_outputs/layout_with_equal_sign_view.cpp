#include "layout_with_equal_sign_view.h"

#include <escher/metric.h>

using namespace Escher;

namespace Calculation {

KDSize LayoutWithEqualSignView::minimalSizeForOptimalDisplay() const {
  if (m_layout.isUninitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = LayoutView::minimalSizeForOptimalDisplay();
  if (!m_showEqual) {
    return expressionSize;
  }
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  return KDSize(
      expressionSize.width() + equalSize.width() + Metric::CommonLargeMargin,
      expressionSize.height());
}

void LayoutWithEqualSignView::drawRect(KDContext* ctx, KDRect rect) const {
  // Do not color the whole background to avoid coloring behind the equal symbol
  LayoutView::drawRect(
      ctx, KDRect(0, 0, LayoutView::minimalSizeForOptimalDisplay()));
}

View* LayoutWithEqualSignView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_equalSign;
}

void LayoutWithEqualSignView::layoutSubviews(bool force) {
  KDSize expressionSize = LayoutView::minimalSizeForOptimalDisplay();
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  KDCoordinate expressionBaseline = layout().baseline(k_font);
  if (!m_showEqual) {
    setChildFrame(&m_equalSign, KDRectZero, force);
  } else {
    setChildFrame(
        &m_equalSign,
        KDRect(expressionSize.width() + Metric::CommonLargeMargin,
               expressionBaseline - equalSize.height() / 2, equalSize),
        force);
  }
}

}  // namespace Calculation

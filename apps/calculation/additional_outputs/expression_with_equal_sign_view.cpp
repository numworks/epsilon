#include "expression_with_equal_sign_view.h"

#include <escher/metric.h>

using namespace Escher;

namespace Calculation {

KDSize ExpressionWithEqualSignView::minimalSizeForOptimalDisplay() const {
  if (m_layout.isUninitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = ExpressionView::minimalSizeForOptimalDisplay();
  if (!m_showEqual) {
    return expressionSize;
  }
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  return KDSize(
      expressionSize.width() + equalSize.width() + Metric::CommonLargeMargin,
      expressionSize.height());
}

void ExpressionWithEqualSignView::drawRect(KDContext* ctx, KDRect rect) const {
  // Do not color the whole background to avoid coloring behind the equal symbol
  ExpressionView::drawRect(
      ctx, KDRect(0, 0, ExpressionView::minimalSizeForOptimalDisplay()));
}

View* ExpressionWithEqualSignView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_equalSign;
}

void ExpressionWithEqualSignView::layoutSubviews(bool force) {
  KDSize expressionSize = ExpressionView::minimalSizeForOptimalDisplay();
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  KDCoordinate expressionBaseline = layout().baseline(k_font);
  if (!m_showEqual) {
    m_equalSign.setFrame(KDRectZero, force);
  } else {
    m_equalSign.setFrame(
        KDRect(expressionSize.width() + Metric::CommonLargeMargin,
               expressionBaseline - equalSize.height() / 2, equalSize),
        force);
  }
}

}  // namespace Calculation

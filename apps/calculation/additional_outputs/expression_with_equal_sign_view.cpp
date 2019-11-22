#include "expression_with_equal_sign_view.h"

namespace Calculation {

KDSize ExpressionWithEqualSignView::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  return KDSize(expressionSize.width() + equalSize.width(), expressionSize.height());
}

View * ExpressionWithEqualSignView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_expressionView;
  }
  assert(index == 1);
  return &m_equalSign;
}

void ExpressionWithEqualSignView::layoutSubviews(bool force) {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  KDSize equalSize = m_equalSign.minimalSizeForOptimalDisplay();
  KDCoordinate expressionBaseline = m_expressionView.layout().baseline();
  m_expressionView.setFrame(KDRect(0, 0, expressionSize), force);
  m_equalSign.setFrame(KDRect(expressionSize.width(), expressionBaseline - equalSize.height()/2, equalSize), force);
}

}

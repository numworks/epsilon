#include <poincare/condensed_sum_layout_node.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

void CondensedSumLayoutNode::computeBaseline() {
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->layoutSize();
  m_baseline = baseLayout()->baseline() + max(0, superscriptSize.height() - baseLayout()->layoutSize().height()/2);
  m_baselined = true;
}

void CondensedSumLayoutNode::computeSize() {
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize subscriptSize = subscriptLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->layoutSize();
  KDCoordinate sizeWidth = baseSize.width() + max(subscriptSize.width(), superscriptSize.width());
  KDCoordinate sizeHeight = max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height());
  m_frame.setSize(KDSize(sizeWidth, sizeHeight));
  m_sized = true;
}

KDPoint CondensedSumLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->layoutSize();
  if (child == baseLayout()) {
    y = max(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == subscriptLayout()) {
    x = baseSize.width();
    y = max(baseSize.height()/2, superscriptSize.height());
  }
  if (child == superscriptLayout()) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

}

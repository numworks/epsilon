#include <poincare/condensed_sum_layout.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

KDCoordinate CondensedSumLayoutNode::computeBaseline() {
  return baseLayout()->baseline() + std::max(0, superscriptLayout()->layoutSize().height() - baseLayout()->layoutSize().height()/2);
}

KDSize CondensedSumLayoutNode::computeSize() {
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize subscriptSize = subscriptLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout()->layoutSize();
  KDCoordinate sizeWidth = baseSize.width() + std::max(subscriptSize.width(), superscriptSize.width());
  KDCoordinate sizeHeight = std::max<KDCoordinate>(baseSize.height()/2, subscriptSize.height()) + std::max<KDCoordinate>(baseSize.height()/2, superscriptSize.height());
  return KDSize(sizeWidth, sizeHeight);
}

KDPoint CondensedSumLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout()->layoutSize();
  if (child == baseLayout()) {
    y = std::max(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == subscriptLayout()) {
    x = baseSize.width();
    y = std::max<KDCoordinate>(baseSize.height()/2, superscriptSize.height());
  }
  if (child == superscriptLayout()) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

}

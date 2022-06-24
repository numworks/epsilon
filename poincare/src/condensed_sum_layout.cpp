#include <poincare/condensed_sum_layout.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

KDCoordinate CondensedSumLayoutNode::computeBaseline(const KDFont * font) {
  return baseLayout()->baseline(font) + std::max(0, superscriptLayout()->layoutSize(font).height() - baseLayout()->layoutSize(font).height()/2);
}

KDSize CondensedSumLayoutNode::computeSize(const KDFont * font) {
  KDSize baseSize = baseLayout()->layoutSize(font);
  KDSize subscriptSize = subscriptLayout()->layoutSize(font);
  KDSize superscriptSize = superscriptLayout()->layoutSize(font);
  KDCoordinate sizeWidth = baseSize.width() + std::max(subscriptSize.width(), superscriptSize.width());
  KDCoordinate sizeHeight = std::max<KDCoordinate>(baseSize.height()/2, subscriptSize.height()) + std::max<KDCoordinate>(baseSize.height()/2, superscriptSize.height());
  return KDSize(sizeWidth, sizeHeight);
}

KDPoint CondensedSumLayoutNode::positionOfChild(LayoutNode * child, const KDFont * font) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->layoutSize(font);
  KDSize superscriptSize = superscriptLayout()->layoutSize(font);
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

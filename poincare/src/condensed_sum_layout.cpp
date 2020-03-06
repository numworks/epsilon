#include <poincare/condensed_sum_layout.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

KDCoordinate CondensedSumLayoutNode::computeBaseline() {
  return baseLayout()->baseline() + maxCoordinate(0, superscriptLayout()->layoutSize().height() - baseLayout()->layoutSize().height()/2);
}

KDSize CondensedSumLayoutNode::computeSize() {
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize subscriptSize = subscriptLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout()->layoutSize();
  KDCoordinate sizeWidth = baseSize.width() + maxCoordinate(subscriptSize.width(), superscriptSize.width());
  KDCoordinate sizeHeight = maxCoordinate(baseSize.height()/2, subscriptSize.height()) + maxCoordinate(baseSize.height()/2, superscriptSize.height());
  return KDSize(sizeWidth, sizeHeight);
}

KDPoint CondensedSumLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout()->layoutSize();
  if (child == baseLayout()) {
    y = maxCoordinate(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == subscriptLayout()) {
    x = baseSize.width();
    y = maxCoordinate(baseSize.height()/2, superscriptSize.height());
  }
  if (child == superscriptLayout()) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

}

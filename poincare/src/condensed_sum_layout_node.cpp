#include <poincare/condensed_sum_layout_node.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }


// TODO remove nullptr in the code?
KDCoordinate CondensedSumLayoutNode::computeBaseline() {
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->layoutSize();
  return baseLayout()->baseline() + max(0, superscriptSize.height() - baseLayout()->layoutSize().height()/2);
}

KDSize CondensedSumLayoutNode::computeSize() {
  KDSize baseSize = baseLayout()->layoutSize();
  KDSize subscriptSize = subscriptLayout()->layoutSize();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->layoutSize();
  KDCoordinate sizeWidth = baseSize.width() + max(subscriptSize.width(), superscriptSize.width());
  KDCoordinate sizeHeight = max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height());
  return KDSize(sizeWidth, sizeHeight);
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

CondensedSumLayoutReference::CondensedSumLayoutReference(LayoutReference base, LayoutReference subscript, LayoutReference superscript) :
  LayoutReference(TreePool::sharedPool()->createTreeNode<CondensedSumLayoutNode>())
{
  replaceChildAtIndexInPlace(0, base);
  replaceChildAtIndexInPlace(1, subscript);
  replaceChildAtIndexInPlace(2, superscript);
}

}

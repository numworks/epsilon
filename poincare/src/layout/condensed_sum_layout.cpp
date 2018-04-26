#include "condensed_sum_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * CondensedSumLayout::clone() const {
  CondensedSumLayout * layout = new CondensedSumLayout(const_cast<CondensedSumLayout *>(this)->baseLayout(), const_cast<CondensedSumLayout *>(this)->subscriptLayout(), const_cast<CondensedSumLayout *>(this)->superscriptLayout(), true);
  return layout;
}

ExpressionLayoutCursor CondensedSumLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  /* CondensedSumLayout is only used in apps/shared/sum_graph_controller.cpp, in
   * a view with no cursor. */
  assert(false);
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor CondensedSumLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  assert(false);
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor CondensedSumLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  assert(false);
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor CondensedSumLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  assert(false);
  return ExpressionLayoutCursor();
}

ExpressionLayout * CondensedSumLayout::layoutToPointWhenInserting() {
  assert(false);
  return nullptr;
}

void CondensedSumLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Nothing to draw
}

KDSize CondensedSumLayout::computeSize() {
  KDSize baseSize = baseLayout()->size();
  KDSize subscriptSize = subscriptLayout()->size();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
  return KDSize(baseSize.width() + max(subscriptSize.width(), superscriptSize.width()), max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height()));
}

void CondensedSumLayout::computeBaseline() {
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
  m_baseline = baseLayout()->baseline() + max(0, superscriptSize.height() - baseLayout()->size().height()/2);
  m_baselined = true;
}

KDPoint CondensedSumLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = baseLayout()->size();
  KDSize superscriptSize = superscriptLayout() == nullptr ? KDSizeZero : superscriptLayout()->size();
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

ExpressionLayout * CondensedSumLayout::baseLayout() {
  return editableChild(0);
}

ExpressionLayout * CondensedSumLayout::subscriptLayout() {
  return editableChild(1);
}

ExpressionLayout * CondensedSumLayout::superscriptLayout() {
  return editableChild(2);
}

}


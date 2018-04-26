#include "binomial_coefficient_layout.h"
#include "empty_layout.h"
#include "grid_layout.h"
#include "horizontal_layout.h"
#include "left_parenthesis_layout.h"
#include "parenthesis_layout.h"
#include "right_parenthesis_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <poincare/expression_layout_array.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * BinomialCoefficientLayout::clone() const {
  return new BinomialCoefficientLayout(const_cast<BinomialCoefficientLayout *>(this)->nLayout(), const_cast<BinomialCoefficientLayout *>(this)->kLayout(), true);
}

ExpressionLayoutCursor BinomialCoefficientLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the children. Go Left.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left
      && (cursor.pointedExpressionLayout() == nLayout()
        || cursor.pointedExpressionLayout() == kLayout()))
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }

  assert(cursor.pointedExpressionLayout() == this);
  // Case: Right. Go to the kLayout.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
    assert(kLayout() != nullptr);
    return ExpressionLayoutCursor(kLayout(), ExpressionLayoutCursor::Position::Right);
  }

  // Case: Left. Ask the parent.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor BinomialCoefficientLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the children. Go Right.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right
      && (cursor.pointedExpressionLayout() == nLayout()
        || cursor.pointedExpressionLayout() == kLayout()))
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }

  assert(cursor.pointedExpressionLayout() == this);
  // Case: Left. Go Left of the nLayout.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
    assert(nLayout() != nullptr);
    return ExpressionLayoutCursor(nLayout(), ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor BinomialCoefficientLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // Case: kLayout. Move to nLayout.
  if (cursor.pointedExpressionLayout()->hasAncestor(kLayout(), true)) {
    return nLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor BinomialCoefficientLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // Case: nLayout. Move to kLayout.
  if (cursor.pointedExpressionLayout()->hasAncestor(nLayout(), true)) {
    return kLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void BinomialCoefficientLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  LeftParenthesisLayout * dummyLeftParenthesis = new LeftParenthesisLayout();
  RightParenthesisLayout * dummyRightParenthesis = new RightParenthesisLayout();
  GridLayout * dummyGridLayout = new GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true);
  HorizontalLayout dummyLayout(dummyLeftParenthesis, dummyGridLayout, dummyRightParenthesis, false);
  KDPoint leftParenthesisPoint = dummyLayout.positionOfChild(dummyLeftParenthesis);
  KDPoint rightParenthesisPoint = dummyLayout.positionOfChild(dummyRightParenthesis);
  dummyLeftParenthesis->render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis->render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

KDSize BinomialCoefficientLayout::computeSize() {
  KDSize coefficientsSize = GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true).size();
  return KDSize(coefficientsSize.width() + 2*ParenthesisLayout::parenthesisWidth(), coefficientsSize.height());
}

void BinomialCoefficientLayout::computeBaseline() {
  m_baseline = GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true).baseline();
  m_baselined = true;
}

KDPoint BinomialCoefficientLayout::positionOfChild(ExpressionLayout * child) {
  LeftParenthesisLayout * dummyLeftParenthesis = new LeftParenthesisLayout();
  RightParenthesisLayout * dummyRightParenthesis = new RightParenthesisLayout();
  GridLayout * dummyGridLayout = new GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true);
  HorizontalLayout dummyLayout(dummyLeftParenthesis, dummyGridLayout, dummyRightParenthesis, false);
  if (child == nLayout()) {
    return dummyGridLayout->positionOfChild(dummyGridLayout->editableChild(0)).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
  }
  assert(child == kLayout());
  return dummyGridLayout->positionOfChild(dummyGridLayout->editableChild(1)).translatedBy(dummyLayout.positionOfChild(dummyGridLayout));
}

ExpressionLayout * BinomialCoefficientLayout::nLayout() {
  return editableChild(0);
}

ExpressionLayout * BinomialCoefficientLayout::kLayout() {
  return editableChild(1);
}

}

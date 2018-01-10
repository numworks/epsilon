#include "binomial_coefficient_layout.h"
#include "empty_visible_layout.h"
#include "grid_layout.h"
#include "horizontal_layout.h"
#include "parenthesis_left_layout.h"
#include "parenthesis_left_right_layout.h"
#include "parenthesis_right_layout.h"
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

void BinomialCoefficientLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // Case: Left of the children.
  // Delete the layout.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && (cursor->pointedExpressionLayout() == nLayout()
        || cursor->pointedExpressionLayout() == kLayout()))
  {
    assert(m_parent != nullptr);
    replaceWithAndMoveCursor(new EmptyVisibleLayout(), true, cursor);
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool BinomialCoefficientLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the children.
  // Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && (cursor->pointedExpressionLayout() == nLayout()
        || cursor->pointedExpressionLayout() == kLayout()))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the kLayout.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(kLayout() != nullptr);
    cursor->setPointedExpressionLayout(kLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }

  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool BinomialCoefficientLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the children.
  // Go Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && (cursor->pointedExpressionLayout() == nLayout()
        || cursor->pointedExpressionLayout() == kLayout()))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go Left of the nLayout.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(nLayout() != nullptr);
    cursor->setPointedExpressionLayout(nLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool BinomialCoefficientLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: kLayout.
  // Move to nLayout.
  if (previousLayout == kLayout()) {
    return nLayout()->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool BinomialCoefficientLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: nLayout.
  // Move to kLayout.
  if (previousLayout == nLayout()) {
    return kLayout()->moveDownInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

void BinomialCoefficientLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  ParenthesisRightLayout * dummyRightParenthesis = new ParenthesisRightLayout();
  GridLayout * dummyGridLayout = new GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true);
  HorizontalLayout dummyLayout(dummyLeftParenthesis, dummyGridLayout, dummyRightParenthesis, false);
  KDPoint leftParenthesisPoint = dummyLayout.positionOfChild(dummyLeftParenthesis);
  KDPoint rightParenthesisPoint = dummyLayout.positionOfChild(dummyRightParenthesis);
  dummyLeftParenthesis->render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis->render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

KDSize BinomialCoefficientLayout::computeSize() {
  KDSize coefficientsSize = GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true).size();
  return KDSize(coefficientsSize.width() + 2*ParenthesisLeftRightLayout::parenthesisWidth(), coefficientsSize.height());
}

void BinomialCoefficientLayout::computeBaseline() {
  m_baseline = GridLayout(ExpressionLayoutArray(nLayout(), kLayout()).array(), 2, 1, true).baseline();
  m_baselined = true;
}

KDPoint BinomialCoefficientLayout::positionOfChild(ExpressionLayout * child) {
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  ParenthesisRightLayout * dummyRightParenthesis = new ParenthesisRightLayout();
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

#include "uneditable_horizontal_trio_layout.h"
#include "empty_visible_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * UneditableHorizontalTrioLayout::clone() const {
  UneditableHorizontalTrioLayout * layout = new UneditableHorizontalTrioLayout(
      const_cast<UneditableHorizontalTrioLayout *>(this)->leftLayout(),
      const_cast<UneditableHorizontalTrioLayout *>(this)->centerLayout(),
      const_cast<UneditableHorizontalTrioLayout *>(this)->rightLayout(),
      true);
  return layout;
}

void UneditableHorizontalTrioLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // Case: Left of the center layout (for sequence layouts).
  if (cursor->pointedExpressionLayout() == centerLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    ExpressionLayout * grandParent = const_cast<ExpressionLayout *>(m_parent->parent());
    assert(grandParent != nullptr);
    ExpressionLayout * parent = m_parent;
    int indexInGrandParent = grandParent->indexOfChild(parent);
    parent->replaceWith(centerLayout(), true);
    // Place the cursor on the right of the left brother of the integral if
    // there is one.
    if (indexInGrandParent > 0) {
      cursor->setPointedExpressionLayout(grandParent->editableChild(indexInGrandParent - 1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return;
    }
    // Else place the cursor on the Left of the parent.
    cursor->setPointedExpressionLayout(grandParent);
    return;
  }
  // Case: Right.
  // Move to the argument.
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    // Go Right of the center layout's last child if it has one, else go Right
    // of the center layout.
    if (centerLayout()->numberOfChildren() > 1) {
      cursor->setPointedExpressionLayout(centerLayout()->editableChild(centerLayout()->numberOfChildren()-1));
      return;
    }
    cursor->setPointedExpressionLayout(centerLayout());
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool UneditableHorizontalTrioLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == centerLayout()) {
    // Case: Center layout.
    // Go Left.
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    // Case: Right.
    // Go Right of the center layout's last child if it has one, else go Right
    // of the center layout.
    if (centerLayout()->numberOfChildren() > 1) {
      cursor->setPointedExpressionLayout(centerLayout()->editableChild(centerLayout()->numberOfChildren()-1));
      return true;
    }
    cursor->setPointedExpressionLayout(centerLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool UneditableHorizontalTrioLayout::moveRight(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == centerLayout()) {
    // Case: Center layout.
    // Go Right.
    assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    // Case: Left.
    // Go Left of the center layout's first child if it has one, else go Left of
    // the center layout.
    ExpressionLayout * grandChild = centerLayout()->editableChild(0);
    if (grandChild != nullptr) {
      cursor->setPointedExpressionLayout(grandChild);
      return true;
    }
    cursor->setPointedExpressionLayout(centerLayout());
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

void UneditableHorizontalTrioLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
}

KDSize UneditableHorizontalTrioLayout::computeSize() {
  // TODO: This code is duplicated from horizontal_layout.cpp.
  KDCoordinate totalWidth = 0;
  int i = 0;
  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;
  while (ExpressionLayout * c = editableChild(i++)) {
    KDSize childSize = c->size();
    totalWidth += childSize.width();
    if (childSize.height() - c->baseline() > max_under_baseline) {
      max_under_baseline = childSize.height() - c->baseline() ;
    }
    if (c->baseline() > max_above_baseline) {
      max_above_baseline = c->baseline();
    }
  }
  return KDSize(totalWidth, max_under_baseline + max_above_baseline);
}

void UneditableHorizontalTrioLayout::computeBaseline() {
  // TODO: This code is duplicated from horizontal_layout.cpp.
  m_baseline = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    if (editableChild(i)->baseline() > m_baseline) {
      m_baseline = editableChild(i)->baseline();
    }
  }
  m_baselined = true;
}

KDPoint UneditableHorizontalTrioLayout::positionOfChild(ExpressionLayout * child) {
  // TODO: This code is duplicated from horizontal_layout.cpp.
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  int index = indexOfChild(child);
  if (index > 0) {
    ExpressionLayout * previousChild = editableChild(index-1);
    assert(previousChild != nullptr);
    x = previousChild->origin().x() + previousChild->size().width();
  }
  y = baseline() - child->baseline();
  return KDPoint(x, y);
}

ExpressionLayout * UneditableHorizontalTrioLayout::leftLayout() {
  return editableChild(0);
}

ExpressionLayout * UneditableHorizontalTrioLayout::centerLayout() {
  return editableChild(1);
}

ExpressionLayout * UneditableHorizontalTrioLayout::rightLayout() {
  return editableChild(2);
}

}

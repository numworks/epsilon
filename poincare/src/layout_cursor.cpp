#include <poincare/layout_cursor.h>
#include <poincare/char_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/matrix_layout.h>
#include <poincare/nth_root_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/charset.h>
#include <stdio.h>

namespace Poincare {

/* Getters and setters */

KDCoordinate LayoutCursor::cursorHeight() {
  KDCoordinate height = layoutHeight();
  return height == 0 ? k_cursorHeight : height;
}

KDCoordinate LayoutCursor::baseline() {
  if (layoutHeight() == 0) {
    return k_cursorHeight/2;
  }
  KDCoordinate layoutBaseline = m_layout.baseline();
  Layout equivalentLayout = m_layout.equivalentCursor(this).layoutReference();
  if (equivalentLayout.isUninitialized()) {
    return layoutBaseline;
  }
  if (m_layout.hasChild(equivalentLayout)) {
    return equivalentLayout.baseline();
  } else if (m_layout.hasSibling(equivalentLayout)) {
    return max(layoutBaseline, equivalentLayout.baseline());
  }
  return layoutBaseline;
}

/* Comparison */

bool LayoutCursor::isEquivalentTo(LayoutCursor cursor) {
  assert(isDefined());
  assert(cursor.isDefined());
  return middleLeftPoint() == cursor.middleLeftPoint();
}

/* Position */

KDPoint LayoutCursor::middleLeftPoint() {
  KDPoint layoutOrigin = layoutReference().absoluteOrigin();
  KDCoordinate x = layoutOrigin.x() + (m_position == Position::Left ? 0 : m_layout.layoutSize().width());
  KDCoordinate y = layoutOrigin.y() + m_layout.baseline() - k_cursorHeight/2;
  return KDPoint(x,y);
}

/* Move */
void LayoutCursor::move(MoveDirection direction, bool * shouldRecomputeLayout) {
  if (direction == MoveDirection::Left) {
    moveLeft(shouldRecomputeLayout);
  } else if (direction == MoveDirection::Right) {
    moveRight(shouldRecomputeLayout);
  } else if (direction == MoveDirection::Up) {
    moveAbove(shouldRecomputeLayout);
  } else if (direction == MoveDirection::Down) {
    moveUnder(shouldRecomputeLayout);
  } else {
    assert(false);
  }
}

/* Layout modification */

void LayoutCursor::addEmptyExponentialLayout() {
  EmptyLayout emptyLayout;
  HorizontalLayout sibling = HorizontalLayout::Builder(
      CharLayout(Ion::Charset::Exponential),
      VerticalOffsetLayout::Builder(emptyLayout, VerticalOffsetLayoutNode::Type::Superscript));
  m_layout.addSibling(this, sibling, false);
  m_layout = emptyLayout;
}

void LayoutCursor::addEmptyMatrixLayout() {
  MatrixLayout matrixLayout = MatrixLayout::Builder(
      EmptyLayout(EmptyLayoutNode::Color::Yellow),
      EmptyLayout(EmptyLayoutNode::Color::Grey),
      EmptyLayout(EmptyLayoutNode::Color::Grey),
      EmptyLayout(EmptyLayoutNode::Color::Grey));
  m_layout.addSibling(this, matrixLayout, false);
  m_layout = matrixLayout.childAtIndex(0);
  m_position = Position::Right;
}

void LayoutCursor::addEmptySquareRootLayout() {
  HorizontalLayout child1 = HorizontalLayout::Builder(EmptyLayout());
  NthRootLayout newChild = NthRootLayout::Builder(child1);
  m_layout.addSibling(this, newChild, false);
  m_layout = newChild.childAtIndex(0);
  ((Layout *)&newChild)->collapseSiblings(this);
}

void LayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(EmptyLayout(), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
  m_layout = offsetLayout.childAtIndex(0);
}

void LayoutCursor::addEmptySquarePowerLayout() {
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(CharLayout('2'), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
}

void LayoutCursor::addEmptyTenPowerLayout() {
  EmptyLayout emptyLayout;
  HorizontalLayout sibling = HorizontalLayout::Builder(
      CharLayout(Ion::Charset::MiddleDot),
      CharLayout('1'),
      CharLayout('0'),
      VerticalOffsetLayout::Builder(
        emptyLayout,
        VerticalOffsetLayoutNode::Type::Superscript));
  m_layout.addSibling(this, sibling, false);
  m_layout = emptyLayout;
}

void LayoutCursor::addFractionLayoutAndCollapseSiblings() {
  HorizontalLayout child1 = HorizontalLayout::Builder(EmptyLayout());
  HorizontalLayout child2 = HorizontalLayout::Builder(EmptyLayout());
  FractionLayout newChild = FractionLayout::Builder(child1, child2);
  m_layout.addSibling(this, newChild, true);
  Layout(newChild.node()).collapseSiblings(this);
}

void LayoutCursor::addXNTCharLayout() {
  m_layout.addSibling(this, CharLayout(m_layout.XNTChar()), true);
}

void LayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  Layout newChild;
  Layout pointedChild;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == Ion::Charset::Empty) {
      continue;
    }
    if (text[i] == Ion::Charset::MultiplicationSign) {
      newChild = CharLayout(Ion::Charset::MiddleDot);
    } else if (text[i] == '(') {
      newChild = LeftParenthesisLayout::Builder();
      if (pointedChild.isUninitialized()) {
        pointedChild = newChild;
      }
    } else if (text[i] == ')') {
      newChild = RightParenthesisLayout::Builder();
    }
    /* We never insert text with brackets for now. Removing this code saves the
     * binary file 2K. */
#if 0
    else if (text[i] == '[') {
      newChild = LeftSquareBracketLayout();
    } else if (text[i] == ']') {
      newChild = RightSquareBracketLayout();
    }
#endif
    else {
      newChild = CharLayout(text[i]);
    }
    m_layout.addSibling(this, newChild, true);
  }
  if (!pointedChild.isUninitialized() && !pointedChild.parent().isUninitialized()) {
    m_layout = pointedChild;
    m_position = Position::Right;
  }
}

void LayoutCursor::addLayoutAndMoveCursor(Layout l) {
  bool layoutWillBeMerged = l.isHorizontal();
  m_layout.addSibling(this, l, true);
  if (!layoutWillBeMerged) {
    l.collapseSiblings(this);
  }
}

void LayoutCursor::clearLayout() {
  Layout rootLayoutR = m_layout.root();
  assert(rootLayoutR.isHorizontal());
  rootLayoutR.removeChildrenInPlace(rootLayoutR.numberOfChildren());
  m_layout = rootLayoutR;
}

/* Private */

KDCoordinate LayoutCursor::layoutHeight() {
  Layout equivalentLayout = m_layout.equivalentCursor(this).layoutReference();
  if (!equivalentLayout.isUninitialized() && m_layout.hasChild(equivalentLayout)) {
    return equivalentLayout.layoutSize().height();
  }
  KDCoordinate pointedLayoutHeight = m_layout.layoutSize().height();
  if (!equivalentLayout.isUninitialized() && m_layout.hasSibling(equivalentLayout)) {
    KDCoordinate equivalentLayoutHeight = equivalentLayout.layoutSize().height();
    KDCoordinate pointedLayoutBaseline = m_layout.baseline();
    KDCoordinate equivalentLayoutBaseline = equivalentLayout.baseline();
    return max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
  }
  return pointedLayoutHeight;
}

void LayoutCursor::privateAddEmptyPowerLayout(VerticalOffsetLayout v) {
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_layout.addSibling(this, v, true);
    return;
  }
  // Else, add an empty base
  EmptyLayout e = EmptyLayout();
  HorizontalLayout newChild = HorizontalLayout::Builder(e, v);
  m_layout.addSibling(this, newChild, true);
}

bool LayoutCursor::baseForNewPowerLayout() {
  /* Returns true if the layout on the left of the pointed layout is suitable to
   * be the base of a new power layout: the base layout should be anything but
   * an horizontal layout with no child. */
  if (m_position == Position::Right) {
    return !(m_layout.isHorizontal() && m_layout.numberOfChildren() == 0);
  } else {
    assert(m_position == Position::Left);
    if (m_layout.isHorizontal()) {
      return false;
    }
    if (m_layout.isEmpty()) {
      /* If the cursor is on the left of an EmptyLayout, move it to its right,
       * the empty layout will take care of changing its color and adding a new
       * row/column to its parent matrix if needed. */
      m_position = Position::Right;
      return true;
    }
    LayoutCursor equivalentLayoutCursor = m_layout.equivalentCursor(this);
    if (equivalentLayoutCursor.layoutReference().isUninitialized()
        || (equivalentLayoutCursor.layoutReference().isHorizontal()
          && equivalentLayoutCursor.position() == Position::Left))
    {
      return false;
    }
    return true;
  }
}

bool LayoutCursor::privateShowHideEmptyLayoutIfNeeded(bool show) {
  /* Find Empty layouts adjacent to the cursor: Check the pointed layout and the
   * equivalent cursor positions */
  Layout adjacentEmptyLayout;

  if (m_layout.isEmpty()) {
    // Check the pointed layout
    adjacentEmptyLayout = m_layout;
  } else {
    // Check the equivalent cursor position
    Layout equivalentPointedLayout = m_layout.equivalentCursor(this).layoutReference();
    if (!equivalentPointedLayout.isUninitialized() && equivalentPointedLayout.isEmpty()) {
      adjacentEmptyLayout = equivalentPointedLayout;
    }
  }

  if (adjacentEmptyLayout.isUninitialized()) {
    return false;
  }
  /* Change the visibility of the neighbouring empty layout: it might be either
   * an EmptyLayout or an HorizontalLayout with one child only, and this child
   * is an EmptyLayout. */
  if (adjacentEmptyLayout.isHorizontal()) {
    static_cast<EmptyLayoutNode *>(adjacentEmptyLayout.childAtIndex(0).node())->setVisible(show);
  } else {
    static_cast<EmptyLayoutNode *>(adjacentEmptyLayout.node())->setVisible(show);
  }
  return true;
}

}

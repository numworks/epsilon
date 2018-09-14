#include <poincare/layout_cursor.h>
#include <poincare/char_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_reference.h>
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
  KDCoordinate layoutBaseline = m_layoutRef.baseline();
  LayoutReference equivalentLayoutReference = m_layoutRef.equivalentCursor(this).layoutReference();
  if (equivalentLayoutReference.isUninitialized()) {
    return layoutBaseline;
  }
  if (m_layoutRef.hasChild(equivalentLayoutReference)) {
    return equivalentLayoutReference.baseline();
  } else if (m_layoutRef.hasSibling(equivalentLayoutReference)) {
    return max(layoutBaseline, equivalentLayoutReference.baseline());
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
  KDCoordinate x = layoutOrigin.x() + (m_position == Position::Left ? 0 : m_layoutRef.layoutSize().width());
  KDCoordinate y = layoutOrigin.y() + m_layoutRef.baseline() - k_cursorHeight/2;
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
  EmptyLayoutReference emptyLayout;
  HorizontalLayoutReference sibling = HorizontalLayoutReference(
      CharLayoutReference(Ion::Charset::Exponential),
      VerticalOffsetLayoutReference(emptyLayout, VerticalOffsetLayoutNode::Type::Superscript));
  m_layoutRef.addSibling(this, sibling, false);
  m_layoutRef = emptyLayout;
}

void LayoutCursor::addEmptyMatrixLayout() {
  MatrixLayoutReference matrixLayout = MatrixLayoutReference(
      EmptyLayoutReference(EmptyLayoutNode::Color::Yellow),
      EmptyLayoutReference(EmptyLayoutNode::Color::Grey),
      EmptyLayoutReference(EmptyLayoutNode::Color::Grey),
      EmptyLayoutReference(EmptyLayoutNode::Color::Grey));
  m_layoutRef.addSibling(this, matrixLayout, false);
  m_layoutRef = matrixLayout.childAtIndex(0);
  m_position = Position::Right;
}

void LayoutCursor::addEmptySquareRootLayout() {
  HorizontalLayoutReference child1 = HorizontalLayoutReference(EmptyLayoutReference());
  NthRootLayoutReference newChild = NthRootLayoutReference(child1);
  m_layoutRef.addSibling(this, newChild, false);
  m_layoutRef = newChild.childAtIndex(0);
  ((LayoutReference *)&newChild)->collapseSiblings(this);
}

void LayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayoutReference offsetLayout = VerticalOffsetLayoutReference(EmptyLayoutReference(), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
  m_layoutRef = offsetLayout.childAtIndex(0);
}

void LayoutCursor::addEmptySquarePowerLayout() {
  VerticalOffsetLayoutReference offsetLayout = VerticalOffsetLayoutReference(CharLayoutReference('2'), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
}

void LayoutCursor::addEmptyTenPowerLayout() {
  EmptyLayoutReference emptyLayout;
  HorizontalLayoutReference sibling = HorizontalLayoutReference(
      CharLayoutReference(Ion::Charset::MiddleDot),
      CharLayoutReference('1'),
      CharLayoutReference('0'),
      VerticalOffsetLayoutReference(
        emptyLayout,
        VerticalOffsetLayoutNode::Type::Superscript));
  m_layoutRef.addSibling(this, sibling, false);
  m_layoutRef = emptyLayout;
}

void LayoutCursor::addFractionLayoutAndCollapseSiblings() {
  HorizontalLayoutReference child1 = HorizontalLayoutReference(EmptyLayoutReference());
  HorizontalLayoutReference child2 = HorizontalLayoutReference(EmptyLayoutReference());
  FractionLayoutReference newChild = FractionLayoutReference(child1, child2);
  m_layoutRef.addSibling(this, newChild, true);
  LayoutReference(newChild.node()).collapseSiblings(this);
}

void LayoutCursor::addXNTCharLayout() {
  m_layoutRef.addSibling(this, CharLayoutReference(m_layoutRef.XNTChar()), true);
}

void LayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  LayoutReference newChild;
  LayoutReference pointedChild;
  bool specialUnderScore = false;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == Ion::Charset::Empty) {
      continue;
    }
    if (text[i] == Ion::Charset::MultiplicationSign) {
      newChild = CharLayoutReference(Ion::Charset::MiddleDot);
    } else if (text[i] == '(') {
      newChild = LeftParenthesisLayoutReference();
      if (pointedChild.isUninitialized()) {
        pointedChild = newChild;
      }
    } else if (text[i] == ')') {
      newChild = RightParenthesisLayoutReference();
    } else if (text[i] == '_') {
      specialUnderScore = (i < textLength) && (text[i+1] == '{');
      if (!specialUnderScore) {
         newChild = CharLayoutReference('_');
      } else {
        continue;
      }
    } else if (text[i] == '{' && specialUnderScore) {
      newChild = CharLayoutReference('('); //TODO ?? Was a char layout before, not a parenthesis left
    } else if (text[i] == '}' && specialUnderScore) {
      newChild = CharLayoutReference(')'); //TODO
      specialUnderScore = false;
    }
    /* We never insert text with brackets for now. Removing this code saves the
     * binary file 2K. */
#if 0
    else if (text[i] == '[') {
      newChild = LeftSquareBracketLayoutReference();
    } else if (text[i] == ']') {
      newChild = RightSquareBracketLayoutReference();
    }
#endif
    else {
      newChild = CharLayoutReference(text[i]);
    }
    m_layoutRef.addSibling(this, newChild, true);
  }
  if (!pointedChild.isUninitialized() && !pointedChild.parent().isUninitialized()) {
    m_layoutRef = pointedChild;
  }
}

void LayoutCursor::addLayoutAndMoveCursor(LayoutReference l) {
  bool layoutWillBeMerged = l.isHorizontal();
  m_layoutRef.addSibling(this, l, true);
  if (!layoutWillBeMerged) {
    l.collapseSiblings(this);
  }
}

void LayoutCursor::clearLayout() {
  LayoutReference rootLayoutR = m_layoutRef.root();
  assert(rootLayoutR.isHorizontal());
  rootLayoutR.removeChildrenInPlace(rootLayoutR.numberOfChildren());
  m_layoutRef = rootLayoutR;
}

/* Private */

KDCoordinate LayoutCursor::layoutHeight() {
  LayoutReference equivalentLayoutReference = m_layoutRef.equivalentCursor(this).layoutReference();
  if (!equivalentLayoutReference.isUninitialized() && m_layoutRef.hasChild(equivalentLayoutReference)) {
    return equivalentLayoutReference.layoutSize().height();
  }
  KDCoordinate pointedLayoutHeight = m_layoutRef.layoutSize().height();
  if (!equivalentLayoutReference.isUninitialized() && m_layoutRef.hasSibling(equivalentLayoutReference)) {
    KDCoordinate equivalentLayoutHeight = equivalentLayoutReference.layoutSize().height();
    KDCoordinate pointedLayoutBaseline = m_layoutRef.baseline();
    KDCoordinate equivalentLayoutBaseline = equivalentLayoutReference.baseline();
    return max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
  }
  return pointedLayoutHeight;
}

void LayoutCursor::privateAddEmptyPowerLayout(VerticalOffsetLayoutReference v) {
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_layoutRef.addSibling(this, v, true);
    return;
  }
  // Else, add an empty base
  EmptyLayoutReference e = EmptyLayoutReference();
  HorizontalLayoutReference newChild = HorizontalLayoutReference(e, v);
  m_layoutRef.addSibling(this, newChild, true);
}

bool LayoutCursor::baseForNewPowerLayout() {
  /* Returns true if the layout on the left of the pointed layout is suitable to
   * be the base of a new power layout: the base layout should be anything but
   * an horizontal layout with no child. */
  if (m_position == Position::Right) {
    return !(m_layoutRef.isHorizontal() && m_layoutRef.numberOfChildren() == 0);
  } else {
    assert(m_position == Position::Left);
    if (m_layoutRef.isHorizontal()) {
      return false;
    }
    if (m_layoutRef.isEmpty()) {
      /* If the cursor is on the left of an EmptyLayout, move it to its right,
       * the empty layout will take care of changing its color and adding a new
       * row/column to its parent matrix if needed. */
      m_position = Position::Right;
      return true;
    }
    LayoutCursor equivalentLayoutCursor = m_layoutRef.equivalentCursor(this);
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
  LayoutReference adjacentEmptyLayout;

  if (m_layoutRef.isEmpty()) {
    // Check the pointed layout
    adjacentEmptyLayout = m_layoutRef;
  } else {
    // Check the equivalent cursor position
    LayoutReference equivalentPointedLayout = m_layoutRef.equivalentCursor(this).layoutReference();
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

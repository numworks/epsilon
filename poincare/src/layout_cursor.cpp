#include <poincare/layout_cursor.h>
#include <poincare/char_layout_node.h>
#include <poincare/empty_layout_node.h>
#include <poincare/fraction_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_reference.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/matrix_layout_node.h>
#include <poincare/nth_root_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <poincare/vertical_offset_layout_node.h>
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
  LayoutRef equivalentLayoutRef = m_layoutRef.equivalentCursor(this).layoutReference();
  if (equivalentLayoutRef.isUninitialized()) {
    return layoutBaseline;
  }
  if (m_layoutRef.hasChild(equivalentLayoutRef)) {
    return equivalentLayoutRef.baseline();
  } else if (m_layoutRef.hasSibling(equivalentLayoutRef)) {
    return max(layoutBaseline, equivalentLayoutRef.baseline());
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
  EmptyLayoutRef emptyLayout;
  HorizontalLayoutRef sibling = HorizontalLayoutRef(
      CharLayoutRef(Ion::Charset::Exponential),
      VerticalOffsetLayoutRef(emptyLayout, VerticalOffsetLayoutNode::Type::Superscript));
  m_layoutRef.addSibling(this, sibling, false);
  m_layoutRef = emptyLayout;
}

void LayoutCursor::addEmptyMatrixLayout() {
  MatrixLayoutRef matrixLayout = MatrixLayoutRef(
      EmptyLayoutRef(EmptyLayoutNode::Color::Yellow),
      EmptyLayoutRef(EmptyLayoutNode::Color::Grey),
      EmptyLayoutRef(EmptyLayoutNode::Color::Grey),
      EmptyLayoutRef(EmptyLayoutNode::Color::Grey));
  m_layoutRef.addSibling(this, matrixLayout, false);
  m_layoutRef = matrixLayout.childAtIndex(0);
  m_position = Position::Right;
}

void LayoutCursor::addEmptySquareRootLayout() {
  HorizontalLayoutRef child1 = HorizontalLayoutRef(EmptyLayoutRef());
  NthRootLayoutRef newChild = NthRootLayoutRef(child1);
  m_layoutRef.addSibling(this, newChild, false);
  m_layoutRef = newChild.childAtIndex(0);
  ((LayoutRef *)&newChild)->collapseSiblings(this);
}

void LayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayoutRef offsetLayout = VerticalOffsetLayoutRef(EmptyLayoutRef(), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
  m_layoutRef = offsetLayout.childAtIndex(0);
}

void LayoutCursor::addEmptySquarePowerLayout() {
  VerticalOffsetLayoutRef offsetLayout = VerticalOffsetLayoutRef(CharLayoutRef('2'), VerticalOffsetLayoutNode::Type::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
}

void LayoutCursor::addEmptyTenPowerLayout() {
  EmptyLayoutRef emptyLayout;
  HorizontalLayoutRef sibling = HorizontalLayoutRef(
      CharLayoutRef(Ion::Charset::MiddleDot),
      CharLayoutRef('1'),
      CharLayoutRef('0'),
      VerticalOffsetLayoutRef(
        emptyLayout,
        VerticalOffsetLayoutNode::Type::Superscript));
  m_layoutRef.addSibling(this, sibling, false);
  m_layoutRef = emptyLayout;
}

void LayoutCursor::addFractionLayoutAndCollapseSiblings() {
  HorizontalLayoutRef child1 = HorizontalLayoutRef(EmptyLayoutRef());
  HorizontalLayoutRef child2 = HorizontalLayoutRef(EmptyLayoutRef());
  FractionLayoutRef newChild = FractionLayoutRef(child1, child2);
  m_layoutRef.addSibling(this, newChild, true);
  LayoutRef(newChild.node()).collapseSiblings(this);
}

void LayoutCursor::addXNTCharLayout() {
  m_layoutRef.addSibling(this, CharLayoutRef(m_layoutRef.XNTChar()), true);
}

void LayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  LayoutRef newChild;
  LayoutRef pointedChild;
  bool specialUnderScore = false;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == Ion::Charset::Empty) {
      continue;
    }
    if (text[i] == Ion::Charset::MultiplicationSign) {
      newChild = CharLayoutRef(Ion::Charset::MiddleDot);
    } else if (text[i] == '(') {
      newChild = LeftParenthesisLayoutRef();
      if (pointedChild.isUninitialized()) {
        pointedChild = newChild;
      }
    } else if (text[i] == ')') {
      newChild = RightParenthesisLayoutRef();
    } else if (text[i] == '_') {
      specialUnderScore = (i < textLength) && (text[i+1] == '{');
      if (!specialUnderScore) {
         newChild = CharLayoutRef('_');
      } else {
        continue;
      }
    } else if (text[i] == '{' && specialUnderScore) {
      newChild = CharLayoutRef('('); //TODO ?? Was a char layout before, not a parenthesis left
    } else if (text[i] == '}' && specialUnderScore) {
      newChild = CharLayoutRef(')'); //TODO
      specialUnderScore = false;
    }
    /* We never insert text with brackets for now. Removing this code saves the
     * binary file 2K. */
#if 0
    else if (text[i] == '[') {
      newChild = LeftSquareBracketLayoutRef();
    } else if (text[i] == ']') {
      newChild = RightSquareBracketLayoutRef();
    }
#endif
    else {
      newChild = CharLayoutRef(text[i]);
    }
    m_layoutRef.addSibling(this, newChild, true);
  }
  if (!pointedChild.isUninitialized() && !pointedChild.parent().isUninitialized()) {
    m_layoutRef = pointedChild;
  }
}

void LayoutCursor::addLayoutAndMoveCursor(LayoutRef l) {
  bool layoutWillBeMerged = l.isHorizontal();
  m_layoutRef.addSibling(this, l, true);
  if (!layoutWillBeMerged) {
    l.collapseSiblings(this);
  }
}

void LayoutCursor::clearLayout() {
  LayoutRef rootLayoutR = m_layoutRef.root();
  assert(rootLayoutR.isHorizontal());
  rootLayoutR.removeChildrenInPlace(rootLayoutR.numberOfChildren());
  m_layoutRef = rootLayoutR;
}

/* Private */

KDCoordinate LayoutCursor::layoutHeight() {
  LayoutRef equivalentLayoutRef = m_layoutRef.equivalentCursor(this).layoutReference();
  if (!equivalentLayoutRef.isUninitialized() && m_layoutRef.hasChild(equivalentLayoutRef)) {
    return equivalentLayoutRef.layoutSize().height();
  }
  KDCoordinate pointedLayoutHeight = m_layoutRef.layoutSize().height();
  if (!equivalentLayoutRef.isUninitialized() && m_layoutRef.hasSibling(equivalentLayoutRef)) {
    KDCoordinate equivalentLayoutHeight = equivalentLayoutRef.layoutSize().height();
    KDCoordinate pointedLayoutBaseline = m_layoutRef.baseline();
    KDCoordinate equivalentLayoutBaseline = equivalentLayoutRef.baseline();
    return max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
  }
  return pointedLayoutHeight;
}

void LayoutCursor::privateAddEmptyPowerLayout(VerticalOffsetLayoutRef v) {
  // If there is already a base
  if (baseForNewPowerLayout()) {
    m_layoutRef.addSibling(this, v, true);
    return;
  }
  // Else, add an empty base
  EmptyLayoutRef e = EmptyLayoutRef();
  HorizontalLayoutRef newChild = HorizontalLayoutRef(e, v);
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
  LayoutRef adjacentEmptyLayout;

  if (m_layoutRef.isEmpty()) {
    // Check the pointed layout
    adjacentEmptyLayout = m_layoutRef;
  } else {
    // Check the equivalent cursor position
    LayoutRef equivalentPointedLayout = m_layoutRef.equivalentCursor(this).layoutReference();
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

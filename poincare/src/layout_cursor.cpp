#include <poincare/layout_cursor.h>
#include <ion/charset.h>
#include <poincare/layout_reference.h>
#include <poincare/char_layout_node.h>
#include <poincare/empty_layout_node.h>
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
  return layoutOrigin;
}

/* Move */

void LayoutCursor::moveLeft(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorLeft(this, shouldRecomputeLayout);
}

void LayoutCursor::moveRight(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorRight(this, shouldRecomputeLayout);
}

void LayoutCursor::moveAbove(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorUp(this, shouldRecomputeLayout);
}

void LayoutCursor::moveUnder(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorDown(this, shouldRecomputeLayout);
}

/* Layout modification */
void LayoutCursor::addXNTCharLayout() {
  m_layoutRef.addSibling(this, CharLayoutRef(m_layoutRef.XNTChar()), true);
}

void LayoutCursor::insertText(const char * text) {
  int textLength = strlen(text);
  if (textLength <= 0) {
    return;
  }
  LayoutRef newChild(nullptr);
  LayoutRef pointedChild(nullptr);
  bool specialUnderScore = false;
  for (int i = 0; i < textLength; i++) {
    if (text[i] == Ion::Charset::Empty) {
      continue;
    }
    if (text[i] == Ion::Charset::MultiplicationSign) {
      newChild = CharLayoutRef(Ion::Charset::MiddleDot);
    } else if (text[i] == '(') {
      newChild = CharLayoutRef('('); //TODO
      if (!pointedChild.isDefined()) {
        pointedChild = newChild;
      }
    } else if (text[i] == ')') {
      newChild = CharLayoutRef(')'); //TODO
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
  if (pointedChild.isDefined() && pointedChild.parent().isDefined()) {
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
  rootLayoutR.removeChildren();
  m_layoutRef = rootLayoutR;
}

/* Private */

KDCoordinate LayoutCursor::layoutHeight() {
  LayoutRef equivalentLayoutRef = m_layoutRef.equivalentCursor(this).layoutReference();
  if (m_layoutRef.hasChild(equivalentLayoutRef)) {
    return equivalentLayoutRef.layoutSize().height();
  }
  KDCoordinate pointedLayoutHeight = m_layoutRef.layoutSize().height();
  if (m_layoutRef.hasSibling(equivalentLayoutRef)) {
    KDCoordinate equivalentLayoutHeight = equivalentLayoutRef.layoutSize().height();
    KDCoordinate pointedLayoutBaseline = m_layoutRef.baseline();
    KDCoordinate equivalentLayoutBaseline = equivalentLayoutRef.baseline();
    return max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
  }
  return pointedLayoutHeight;

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
       * make sure it is yellow, and this EmptyLayout will be the base of the
       * new power layout. */
      m_position = Position::Right;
      if (static_cast<EmptyLayoutNode *>(m_layoutRef.node())->color() == EmptyLayoutNode::Color::Grey) {
        static_cast<EmptyLayoutNode *>(m_layoutRef.node())->setColor(EmptyLayoutNode::Color::Yellow);
        LayoutRef p = m_layoutRef.parent();
        int idxInParent = p.indexOfChild(m_layoutRef);
        assert(idxInParent >= 0);
        //TODO static_cast<MatrixLayout *>(p.node())->newRowOrColumnAtIndex(idxInParent);
      }
      return true;
    }
    LayoutCursor equivalentLayoutCursor = m_layoutRef.equivalentCursor(this);
    if (!equivalentLayoutCursor.layoutReference().isDefined()
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
  LayoutRef adjacentEmptyLayout(nullptr);

  if (m_layoutRef.isEmpty()) {
    // Check the pointed layout
    adjacentEmptyLayout = m_layoutRef;
  } else {
    // Check the equivalent cursor position
    LayoutRef equivalentPointedLayout = m_layoutRef.equivalentCursor(this).layoutReference();
    if (equivalentPointedLayout.isDefined() && equivalentPointedLayout.isEmpty()) {
      adjacentEmptyLayout = equivalentPointedLayout;
    }
  }

  if (!adjacentEmptyLayout.isDefined()) {
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

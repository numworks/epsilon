#include <poincare/layout_cursor.h>
#include <poincare/code_point_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/matrix_layout.h>
#include <poincare/nth_root_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <algorithm>

namespace Poincare {

/* Getters and setters */

KDCoordinate LayoutCursor::cursorHeightWithoutSelection(KDFont::Size font) {
  KDCoordinate height = layoutHeight(font);
  return height == 0 ? k_cursorHeight : height;
}

KDCoordinate LayoutCursor::baselineWithoutSelection(KDFont::Size font) {
  if (layoutHeight(font) == 0) {
    return k_cursorHeight/2;
  }
  KDCoordinate layoutBaseline = m_layout.baseline(font);
  Layout equivalentLayout = m_layout.equivalentCursor(this).layout();
  if (equivalentLayout.isUninitialized()) {
    return layoutBaseline;
  }
  if (m_layout.hasChild(equivalentLayout)) {
    return equivalentLayout.baseline(font);
  } else if (m_layout.hasSibling(equivalentLayout)) {
    return std::max(layoutBaseline, equivalentLayout.baseline(font));
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
  /* Font consistency :
   *  By not storing the font as member variable, and not passing it in the
   *  signature either, we suppose here that :
   *   - Comparing middleLeftPoints in the wrong font is be equivalent
   *   - Asking for the LargeFont's absoluteOrigin/layoutSize/baseline of a
   *     layout usually displayed in SmallFont is handled. */
  KDPoint layoutOrigin = layout().absoluteOrigin(KDFont::Size::Large);
  KDCoordinate x = layoutOrigin.x() + (m_position == Position::Left ? 0 : m_layout.layoutSize(KDFont::Size::Large).width());
  KDCoordinate y = layoutOrigin.y() + m_layout.baseline(KDFont::Size::Large) - k_cursorHeight/2;
  return KDPoint(x,y);
}

/* Move */
void LayoutCursor::move(Direction direction, bool * shouldRecomputeLayout, bool forSelection) {
  if (direction == Direction::Left) {
    moveLeft(shouldRecomputeLayout, forSelection);
  } else if (direction == Direction::Right) {
    moveRight(shouldRecomputeLayout, forSelection);
  } else if (direction == Direction::Up) {
    moveAbove(shouldRecomputeLayout, forSelection);
  } else {
    assert(direction == Direction::Down);
    moveUnder(shouldRecomputeLayout, forSelection);
  }
}

LayoutCursor LayoutCursor::cursorAtDirection(Direction direction, bool * shouldRecomputeLayout, bool forSelection, int step) {
  LayoutCursor result = clone();
  if (step <= 0) {
    return result;
  }
  // First step
  result.move(direction, shouldRecomputeLayout, forSelection);

  if (step == 1 || !result.isDefined()) {
    // If first step is undefined, it is returned so the situation is handled
    return result;
  }
  // Otherwise, as many steps as possible are performed
  LayoutCursor result_temp = result;
  for (int i = 1; i < step; ++i) {
    result_temp.move(direction, shouldRecomputeLayout, forSelection);
    if (!result_temp.isDefined()) {
      // Return last successful result
      return result;
    }
    // Update last successful result
    result = result_temp;
    assert(result.isDefined());
  }
  return result;
}

/* Select */

void LayoutCursor::select(Direction direction, bool * shouldRecomputeLayout, Layout * selection) {
  if (direction == Direction::Right || direction == Direction::Left) {
    selectLeftRight(direction == Direction::Right, shouldRecomputeLayout, selection);
  } else {
    selectUpDown(direction == Direction::Up, shouldRecomputeLayout, selection);
  }
}

LayoutCursor LayoutCursor::selectAtDirection(Direction direction, bool * shouldRecomputeLayout, Layout * selection) {
  LayoutCursor result = clone();
  result.select(direction, shouldRecomputeLayout, selection);
  return result;
}

/* Layout modification */

void LayoutCursor::addEmptyExponentialLayout() {
  EmptyLayout emptyLayout = EmptyLayout::Builder();
  HorizontalLayout sibling = HorizontalLayout::Builder(
      CodePointLayout::Builder('e'),
      VerticalOffsetLayout::Builder(emptyLayout, VerticalOffsetLayoutNode::Position::Superscript));
  m_layout.addSibling(this, sibling, false);
  m_layout = emptyLayout;
}

void LayoutCursor::addEmptyMatrixLayout() {
  MatrixLayout matrixLayout = MatrixLayout::Builder(
      EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow),
      EmptyLayout::Builder(EmptyLayoutNode::Color::Gray),
      EmptyLayout::Builder(EmptyLayoutNode::Color::Gray),
      EmptyLayout::Builder(EmptyLayoutNode::Color::Gray));
  m_layout.addSibling(this, matrixLayout, false);
  m_layout = matrixLayout.childAtIndex(0);
  m_position = Position::Right;
}

void LayoutCursor::addEmptySquareRootLayout() {
  // TODO: add a horizontal layout only if several children
  HorizontalLayout child1 = HorizontalLayout::Builder(EmptyLayout::Builder());
  NthRootLayout newChild = NthRootLayout::Builder(child1);
  m_layout.addSibling(this, newChild, false);
  m_layout = newChild.childAtIndex(0);
  m_position = Position::Left;
  ((Layout *)&newChild)->collapseSiblings(this);
}

void LayoutCursor::addEmptyPowerLayout() {
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(EmptyLayout::Builder(), VerticalOffsetLayoutNode::Position::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
  m_layout = offsetLayout.childAtIndex(0);
}

void LayoutCursor::addEmptySquarePowerLayout() {
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'), VerticalOffsetLayoutNode::Position::Superscript);
  privateAddEmptyPowerLayout(offsetLayout);
  m_layout = offsetLayout;
  m_position = Position::Right;
}

void LayoutCursor::addEmptyTenPowerLayout() {
  EmptyLayout emptyLayout = EmptyLayout::Builder();
  HorizontalLayout sibling = HorizontalLayout::Builder(
      CodePointLayout::Builder(UCodePointMultiplicationSign),
      CodePointLayout::Builder('1'),
      CodePointLayout::Builder('0'),
      VerticalOffsetLayout::Builder(
        emptyLayout,
        VerticalOffsetLayoutNode::Position::Superscript));
  m_layout.addSibling(this, sibling, false);
  m_layout = emptyLayout;
}

void LayoutCursor::addFractionLayoutAndCollapseSiblings() {
  HorizontalLayout child1 = HorizontalLayout::Builder(EmptyLayout::Builder());
  HorizontalLayout child2 = HorizontalLayout::Builder(EmptyLayout::Builder());
  FractionLayout newChild = FractionLayout::Builder(child1, child2);
  m_layout.addSibling(this, newChild, true);
  Layout(newChild.node()).collapseSiblings(this);
}

void LayoutCursor::insertText(const char * text, bool forceCursorRightOfText, bool forceCursorLeftOfText) {
  Layout newChild;
  Layout pointedChild;
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  if (codePoint == UCodePointNull) {
    return;
  }
  assert(!codePoint.isCombining());
  while (codePoint != UCodePointNull) {
    if (codePoint == UCodePointEmpty) {
      codePoint = decoder.nextCodePoint();
      assert(!codePoint.isCombining());
      continue;
    }
    AutocompletedBracketPairLayoutNode::Side bracketSide = AutocompletedBracketPairLayoutNode::Side::Left;
    if (codePoint == UCodePointMultiplicationSign) {
      newChild = CodePointLayout::Builder(UCodePointMultiplicationSign);
    } else if (codePoint == '(' || codePoint == UCodePointLeftSystemParenthesis) {
      newChild = ParenthesisLayout::Builder();
    } else if (codePoint == ')' || codePoint == UCodePointRightSystemParenthesis) {
      newChild = ParenthesisLayout::Builder();
      bracketSide = AutocompletedBracketPairLayoutNode::Side::Right;
    } else if (codePoint == '{') {
      newChild = CurlyBraceLayout::Builder();
    } else if (codePoint == '}') {
      newChild = CurlyBraceLayout::Builder();
      bracketSide = AutocompletedBracketPairLayoutNode::Side::Right;
    } else {
      newChild = CodePointLayout::Builder(codePoint);
    }
    if (forceCursorLeftOfText && pointedChild.isUninitialized()) {
      // Point to first non empty codePoint inserted
      pointedChild = newChild;
    }
    m_layout.addSibling(this, newChild, true);

    if (newChild.type() == LayoutNode::Type::ParenthesisLayout || newChild.type() == LayoutNode::Type::CurlyBraceLayout) {
      AutocompletedBracketPairLayoutNode * autocompletedBracket = static_cast<AutocompletedBracketPairLayoutNode *>(newChild.node());
      autocompletedBracket->balanceAfterInsertion(bracketSide, this);
    }

    // Get the next code point
    codePoint = decoder.nextCodePoint();
    while (codePoint.isCombining()) {
      codePoint = decoder.nextCodePoint();
    }
  }
  if (!forceCursorRightOfText && !pointedChild.isUninitialized() && !pointedChild.parent().isUninitialized()) {
    m_layout = pointedChild;
    m_position = forceCursorLeftOfText ? Position::Left : Position::Right;
  }
}

void LayoutCursor::addLayoutAndMoveCursor(Layout l) {
  bool layoutWillBeMerged = l.type() == LayoutNode::Type::HorizontalLayout;
  m_layout.addSibling(this, l, true);
  if (!layoutWillBeMerged) {
    l.collapseSiblings(this);
  }
}

void LayoutCursor::clearLayout() {
  Layout rootLayoutR = m_layout.root();
  assert(rootLayoutR.type() == LayoutNode::Type::HorizontalLayout);
  rootLayoutR.removeChildrenInPlace(rootLayoutR.numberOfChildren());
  m_layout = rootLayoutR;
}

/* Private */

KDCoordinate LayoutCursor::layoutHeight(KDFont::Size font) {
  Layout equivalentLayout = m_layout.equivalentCursor(this).layout();
  if (!equivalentLayout.isUninitialized() && m_layout.hasChild(equivalentLayout)) {
    return equivalentLayout.layoutSize(font).height();
  }
  KDCoordinate pointedLayoutHeight = m_layout.layoutSize(font).height();
  if (!equivalentLayout.isUninitialized() && m_layout.hasSibling(equivalentLayout)) {
    KDCoordinate equivalentLayoutHeight = equivalentLayout.layoutSize(font).height();
    KDCoordinate pointedLayoutBaseline = m_layout.baseline(font);
    KDCoordinate equivalentLayoutBaseline = equivalentLayout.baseline(font);
    return std::max(pointedLayoutBaseline, equivalentLayoutBaseline)
      + std::max(pointedLayoutHeight - pointedLayoutBaseline, equivalentLayoutHeight - equivalentLayoutBaseline);
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
  EmptyLayout e = EmptyLayout::Builder();
  HorizontalLayout newChild = HorizontalLayout::Builder(e, v);
  m_layout.addSibling(this, newChild, true);
}

bool LayoutCursor::baseForNewPowerLayout() {
  /* Returns true if the layout on the left of the pointed layout is suitable to
   * be the base of a new power layout: the base layout should be anything but
   * an horizontal layout with no child. */
  if (m_position == Position::Right) {
    return !(m_layout.type() == LayoutNode::Type::HorizontalLayout && m_layout.numberOfChildren() == 0);
  } else {
    assert(m_position == Position::Left);
    if (m_layout.type() == LayoutNode::Type::HorizontalLayout) {
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
    if (equivalentLayoutCursor.layout().isUninitialized()
        || (equivalentLayoutCursor.layout().type() == LayoutNode::Type::HorizontalLayout
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
    Layout equivalentPointedLayout = m_layout.equivalentCursor(this).layout();
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
  if (adjacentEmptyLayout.type() == LayoutNode::Type::HorizontalLayout) {
    static_cast<EmptyLayoutNode *>(adjacentEmptyLayout.childAtIndex(0).node())->setVisible(show);
  } else {
    static_cast<EmptyLayoutNode *>(adjacentEmptyLayout.node())->setVisible(show);
  }
  return true;
}

void LayoutCursor::selectLeftRight(bool right, bool * shouldRecomputeLayout, Layout * selection) {
  assert(!m_layout.isUninitialized());

  // Compute ingoing / outgoing positions
  Position ingoingPosition = right ? Position::Left : Position::Right;
  Position outgoingPosition = right ? Position::Right : Position::Left;

  // Handle empty layouts
  bool currentLayoutIsEmpty = m_layout.type() == LayoutNode::Type::EmptyLayout;
  if (currentLayoutIsEmpty) {
    m_position = outgoingPosition;
  }

  // Find the layout to select
  LayoutCursor equivalentCursor = m_layout.equivalentCursor(this);
  Layout equivalentLayout = equivalentCursor.layout();

  if (m_position == ingoingPosition) {
    /* The current cursor is positionned on the ingoing position, for instance
     * left a layout if we want to select towards the right. */
    if (!equivalentLayout.isUninitialized() && m_layout.hasChild(equivalentLayout)) {
      /* Put the cursor on the inner most equivalent ingoing position: for
       * instance, in the layout   |1234    , the cursor should be left of the 1,
       * not left of the horizontal layout. */
      assert(equivalentCursor.position() == ingoingPosition);
      *selection = equivalentLayout;
    } else {
      /* If there is no adequate equivalent position, just set the ingoing
       * layout on the current layout. */
      *selection = m_layout;
    }
  } else {
    /* The cursor is on the outgoing position, for instance right of a layout
     * when we want to select towards the right. */
    if (!equivalentLayout.isUninitialized() && equivalentCursor.position() == ingoingPosition) {
      /* If there is an equivalent layout positionned on the ingoing position,
       * try the algorithm with it. */
      assert(equivalentLayout.type() != LayoutNode::Type::HorizontalLayout);
      m_layout = equivalentLayout;
      m_position = ingoingPosition;
      selectLeftRight(right, shouldRecomputeLayout, selection);
      return;
    } else {
      // Else, find the first non horizontal ancestor and select it.
      Layout notHorizontalAncestor = m_layout.parent();
      while (!notHorizontalAncestor.isUninitialized()
          && notHorizontalAncestor.type() == LayoutNode::Type::HorizontalLayout)
      {
        notHorizontalAncestor = notHorizontalAncestor.parent();
      }
      if (notHorizontalAncestor.isUninitialized()) {
        return; // Leave selection empty
      }
      *selection = notHorizontalAncestor;
    }
  }
  m_layout = *selection;
  m_position = outgoingPosition;
}

void LayoutCursor::selectUpDown(bool up, bool * shouldRecomputeLayout, Layout * selection) {
  // Move the cursor in the selection direction
  Layout p = m_layout.parent();
  LayoutCursor c = cursorAtDirection(up ? Direction::Up : Direction::Down, shouldRecomputeLayout, true);
  if (!c.isDefined()) {
    return;
  }

  /* Find the first common ancestor between the current layout and the layout of
   * the moved cursor (also check the common ancestor with the equivalent
   * position). This ancestor will be the added selection.
   *
   * The current layout might have been detached from its parent, for instance
   * if it was a gray empty layout of a matrix and the cursor move exited this
   * matrix. In this case, use the layout parent (it should still be attached to
   * the main layout). */

  const bool previousLayoutWasDetached = m_layout.parent() != p;
  Layout previousCursoredLayout = previousLayoutWasDetached ? p : m_layout;
  TreeHandle ancestor1 = previousCursoredLayout.commonAncestorWith(c.layout());
  TreeHandle ancestor2 = Layout();
  LayoutCursor eqCursor;
  if (!previousLayoutWasDetached) {
    eqCursor = previousCursoredLayout.equivalentCursor(this);
    Layout equivalentLayout = eqCursor.layout();
    if (!equivalentLayout.isUninitialized()) {
      ancestor2 = equivalentLayout.commonAncestorWith(c.layout());
    }
  }
  // Select the closest common ancestor
  bool ancestorOfPointedLayoutSelected = ancestor2.isUninitialized() || !ancestor2.hasAncestor(ancestor1, true);
  TreeHandle ancestor = ancestorOfPointedLayoutSelected ? ancestor1 : ancestor2;
  *selection = static_cast<Layout &>(ancestor);
  LayoutCursor * usedCursor = ancestorOfPointedLayoutSelected ? this : &eqCursor;

  if (usedCursor->layout() == *selection) {
    /* Example:
     *    415
     * 89|--- + 1 -> If the cursor is left of the fraction and we select up, we
     *     2         want to add the whole fraction to the selection.
     *          The fraction is the common ancestor between the pointed layout
     *          (or its equivalent layout) and the layout pointed after the move
     *          (either the horizontal layout containing "415", or the layout
     *          "4"). */
    m_position = usedCursor->position() == Position::Right ? Position::Left : Position::Right;
  } else {
    /* We choose arbitrarily to select towards the left if we go up, and towards
     * the right if we go down.
     * Example:
     * 415
     * --- -> If the 2 is selected and we select up, we select the whole
     * |2     fraction towards the left.  */
    m_position = up ? Position::Left : Position::Right;
  }
  m_layout = *selection;
}

}

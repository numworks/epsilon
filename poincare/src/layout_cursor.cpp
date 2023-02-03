#include <poincare/layout_cursor.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/horizontal_layout.h>
//#include <poincare/input_beautification.h>
#include <poincare/layout.h>
#include <poincare/parenthesis_layout.h>
//#include <poincare/matrix_layout.h>
#include <poincare/nth_root_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <algorithm>

namespace Poincare {

/* Getters and setters */

KDCoordinate LayoutCursor::cursorHeight(KDFont::Size font) {
  LayoutSelection currentSelection = selection();
  if (currentSelection.isEmpty()) {
    Layout baseLayout = layoutToFit(font);
    return layoutToFit(font).layoutSize(font).height();
  }

  if (m_layout.isHorizontal()) {
    return static_cast<HorizontalLayoutNode *>(m_layout.node())->layoutSizeBetweenIndexes(currentSelection.leftPosition(), currentSelection.rightPosition(), font).height();
  }

  return m_layout.layoutSize(font).height();
}

KDPoint LayoutCursor::cursorAbsoluteOrigin(KDFont::Size font) {
  KDCoordinate cursorBaseline = 0;
  LayoutSelection currentSelection = selection();
  if (!currentSelection.isEmpty() && m_layout.isHorizontal()) {
    cursorBaseline = static_cast<HorizontalLayout&>(m_layout).baselineBetweenIndexes(currentSelection.leftPosition(), currentSelection.rightPosition(), font);
  } else {
    cursorBaseline = layoutToFit(font).baseline(font);
  }
  KDCoordinate cursorYOriginInLayout = m_layout.baseline(font) - cursorBaseline;
  KDCoordinate cursorXOffset = 0;
  if (m_layout.isHorizontal()) {
    cursorXOffset = static_cast<HorizontalLayout&>(m_layout).layoutSizeBetweenIndexes(0, m_position, font).width();
  } else {
    cursorXOffset = m_position == 1 ? m_layout.layoutSize(font).width() : 0;
  }
  return m_layout.absoluteOrigin(font).translatedBy(KDPoint(cursorXOffset, cursorYOriginInLayout));
}

/* Move */
bool LayoutCursor::move(OMG::Direction direction, bool selecting, bool * shouldRedrawLayout) {
  *shouldRedrawLayout = false;
  if (!selecting && isSelecting()) {
    stopSelecting();
    *shouldRedrawLayout = true;
    return true;
  }
  if (selecting && !isSelecting()) {
    privateStartSelecting();
  }
  LayoutCursor cloneCursor = *this;
  bool moved = false;
  // TODO: Factorize these check and set of direction
  if (direction == OMG::Direction::Up || direction == OMG::Direction::Down) {
    moved = privateVerticalMove(direction == OMG::Direction::Up ? OMG::VerticalDirection::Up : OMG::VerticalDirection::Down, shouldRedrawLayout);
  } else {
    moved = privateHorizontalMove(direction == OMG::Direction::Left ? OMG::HorizontalDirection::Left : OMG::HorizontalDirection::Right, shouldRedrawLayout);
  }
  if (moved) {
    *shouldRedrawLayout = selecting || *shouldRedrawLayout;
    // Ensure that willExit and didEnter are always called by being left of ||
    *shouldRedrawLayout = cloneCursor.willExitCurrentPosition() || *shouldRedrawLayout;
    *shouldRedrawLayout = didEnterCurrentPosition() || *shouldRedrawLayout;
  }
  return moved;
}

bool LayoutCursor::moveMultipleSteps(OMG::Direction direction, int step, bool selecting, bool * shouldRedrawLayout) {
  assert(step > 0);
  for (int i = 0; i < step; i++) {
    if (!move(direction, selecting, shouldRedrawLayout)) {
      return i > 0;
    }
  }
  return true;
}

/* Layout modification */
void LayoutCursor::insertLayoutAtCursor(Layout layout, Context * context, bool forceRight) {
  assert(!isUninitialized() && isValid());
  deleteAndResetSelection();
  if (m_layout.isHorizontal()) {
    int positionShift = layout.isHorizontal() ? layout.numberOfChildren() : 1;
    static_cast<HorizontalLayout&>(m_layout).addOrMergeChildAtIndex(layout, m_position);
    m_position += positionShift;
  } else {
    assert(m_layout.parent().isUninitialized() || !m_layout.parent().isHorizontal());
    HorizontalLayout newParent = HorizontalLayout::Builder();
    m_layout.replaceWithInPlace(newParent);
    Layout leftLayout = m_position == 0 ? layout : m_layout;
    Layout rightLayout = m_position == 1 ? m_layout : layout;
    newParent.addOrMergeChildAtIndex(leftLayout, 0);
    newParent.addOrMergeChildAtIndex(rightLayout, newParent.numberOfChildren());
    m_layout =  newParent;
    m_position = m_layout.numberOfChildren() - (m_position == 0);
  }
  invalidateSizesAndPositions();
}

void LayoutCursor::addEmptyExponentialLayout(Context * context) {
  insertLayoutAtCursor(
    HorizontalLayout::Builder(
      CodePointLayout::Builder('e'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript)
    ),
    context);
}

void LayoutCursor::addEmptyMatrixLayout(Context * context) {
  //insertLayoutAtCursor(MatrixLayout::EmptySquaredMatrixBuilder(), context);
}

void LayoutCursor::addEmptySquareRootLayout(Context * context) {
  insertLayoutAtCursor(NthRootLayout::Builder(HorizontalLayout::Builder()), context);
}

void LayoutCursor::addEmptyPowerLayout(Context * context) {
  insertLayoutAtCursor(
    VerticalOffsetLayout::Builder(
      HorizontalLayout::Builder(),
      VerticalOffsetLayoutNode::VerticalPosition::Superscript),
    context);
}

void LayoutCursor::addEmptySquarePowerLayout(Context * context) {
  insertLayoutAtCursor(
    VerticalOffsetLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayoutNode::VerticalPosition::Superscript),
    context);
}

void LayoutCursor::addEmptyTenPowerLayout(Context * context) {
  insertLayoutAtCursor(
    HorizontalLayout::Builder({
      CodePointLayout::Builder(UCodePointMultiplicationSign),
      CodePointLayout::Builder('1'),
      CodePointLayout::Builder('0'),
      VerticalOffsetLayout::Builder(
        HorizontalLayout::Builder(),
        VerticalOffsetLayoutNode::VerticalPosition::Superscript)}),
    context);
}

void LayoutCursor::addFractionLayoutAndCollapseSiblings(Context * context) {
  // TODO: Restore collapse sibling behaviour
  insertLayoutAtCursor(
    FractionLayout::Builder(
      HorizontalLayout::Builder(),
      HorizontalLayout::Builder()),
    context);
}

void LayoutCursor::insertText(const char * text, Context * context, bool forceCursorRightOfText, bool forceCursorLeftOfText) {
  // TODO: Restore parenthesis and subscript behaviour
  int currentPosition = m_position;
  insertLayoutAtCursor(LayoutHelper::StringToCodePointsLayout(text, strlen(text)), context, forceCursorRightOfText);
  if (forceCursorLeftOfText) {
    m_position = currentPosition;
  }
  // TODO: Restore beautification
}

void LayoutCursor::performBackspace() {
  invalidateSizesAndPositions();
  if (isSelecting()) {
    deleteAndResetSelection();
    return;
  }
  // TODO: Restore deletion behaviour
}

void LayoutCursor::deleteAndResetSelection() {
  LayoutSelection selec = selection();
  if (selec.isEmpty()) {
    return;
  }
  int selectionLeftBound = selec.leftPosition();
  int selectionRightBound = selec.rightPosition();
  if (m_layout.isHorizontal()) {
    for (int i = selectionLeftBound; i < selectionRightBound; i++) {
      static_cast<HorizontalLayout&>(m_layout).removeChildAtIndexInPlace(selectionLeftBound);
    }
  } else {
    assert(m_layout.parent().isUninitialized() || !m_layout.parent().isHorizontal());
    m_layout.replaceWithInPlace(HorizontalLayout::Builder());
  }
  m_position = selectionLeftBound;
  stopSelecting();
  didEnterCurrentPosition();
}

bool LayoutCursor::willExitCurrentPosition() {
  if (isUninitialized()) {
    return false;
  }
  bool changed = setEmptyRectangleVisibility(EmptyRectangle::State::Visible);
  if (changed) {
    invalidateSizesAndPositions();
  }
  return changed;
}

bool LayoutCursor::didEnterCurrentPosition() {
  if (isUninitialized()) {
    return false;
  }
  bool changed = setEmptyRectangleVisibility(EmptyRectangle::State::Hidden);
  if (changed) {
    invalidateSizesAndPositions();
  }
  return changed;
}

bool LayoutCursor::isAtNumeratorOfEmptyFraction() const {
  return
    m_layout.numberOfChildren() == 0 &&
    !m_layout.parent().isUninitialized() &&
    m_layout.parent().type() == LayoutNode::Type::FractionLayout &&
    m_layout.parent().indexOfChild(m_layout) == 0 &&
    m_layout.parent().childAtIndex(1).numberOfChildren() == 0;
}

/* Private */

Layout LayoutCursor::leftLayout() {
  assert(!isUninitialized());
  if (!m_layout.isHorizontal()) {
    return m_position == 1 ? m_layout : Layout();
  }
  if (m_layout.numberOfChildren() == 0 || m_position == 0) {
    return Layout();
  }
  return m_layout.childAtIndex(m_position - 1);
}

Layout LayoutCursor::rightLayout() {
  assert(!isUninitialized());
  if (!m_layout.isHorizontal()) {
    return m_position == 0 ? m_layout : Layout();
  }
  if (m_layout.numberOfChildren() == 0 || m_position == m_layout.numberOfChildren()) {
    return Layout();
  }
  return m_layout.childAtIndex(m_position);
}

Layout LayoutCursor::layoutToFit(KDFont::Size font) {
  assert(!isUninitialized());
  Layout leftL = leftLayout();
  Layout rightL = rightLayout();
  if (leftL.isUninitialized() && rightL.isUninitialized()) {
    return m_layout;
  }
  return leftL.isUninitialized() || (!rightL.isUninitialized() && leftL.layoutSize(font).height() < rightL.layoutSize(font).height()) ? rightL : leftL;
}

void LayoutCursor::setPositionAtEndpointAfterHorizontalMove(OMG::HorizontalDirection direction) {
  m_position = direction == OMG::HorizontalDirection::Right ? 0 : (m_layout.isHorizontal() ? m_layout.numberOfChildren() : 1);
}


bool LayoutCursor::privateHorizontalMove(OMG::HorizontalDirection direction, bool * shouldRedrawLayout) {
  Layout nextLayout = Layout();
  int currentIndexInNextLayout = LayoutNode::k_outsideIndex;
  bool selecting = isSelecting();

  if (direction == OMG::HorizontalDirection::Right) {
    nextLayout = rightLayout();
  } else {
    nextLayout = leftLayout();
  }

  if (nextLayout.isUninitialized()) {
    /* If nextLayout is uninitialized, the cursor is at the left-most or
     * right-most position. It should move to the parent. */
    if (m_layout.parent().isUninitialized()) {
      return false;
    }
    nextLayout = m_layout.parent();
    currentIndexInNextLayout = nextLayout.indexOfChild(m_layout);
  }
  assert(!nextLayout.isUninitialized());
  assert(!nextLayout.isHorizontal());

  int newIndex = selecting ? LayoutNode::k_outsideIndex : nextLayout.indexOfNextChildToPointToAfterHorizontalCursorMove(direction, currentIndexInNextLayout);
  if (newIndex == LayoutNode::k_outsideIndex) {
    Layout parent = nextLayout.parent();
    Layout previousLayout = m_layout;
    if (!parent.isUninitialized() && parent.isHorizontal()) {
      m_layout = nextLayout.parent();
      m_position = m_layout.indexOfChild(nextLayout) + (direction == OMG::HorizontalDirection::Right);
    } else {
      m_layout = nextLayout;
      m_position = direction == OMG::HorizontalDirection::Right;
    }
    if (selecting && m_layout != previousLayout) {
      m_startOfSelection = m_position + (direction == OMG::HorizontalDirection::Right ? -1 : 1);
    }
    return true;
  }
  // Enter the next layout child
  m_layout = nextLayout.childAtIndex(newIndex);
  setPositionAtEndpointAfterHorizontalMove(direction);
  return true;
}

bool LayoutCursor::privateVerticalMove(OMG::VerticalDirection direction, bool * shouldRedrawLayout) {
  int currentIndexInNextLayout = LayoutNode::k_outsideIndex;
  // TODO: Handle selection
  bool selecting = isSelecting();

  int nextIndex = LayoutNode::k_outsideIndex;

  /* Step 1:
   * Try to enter right or left layout if it can be entered through up/down */
  Layout rightL = rightLayout();
  if (!rightL.isUninitialized()) {
    nextIndex = rightL.indexOfNextChildToPointToAfterVerticalCursorMove(direction, LayoutNode::k_outsideIndex, LayoutNode::PositionInLayout::Left);
    if (nextIndex != LayoutNode::k_outsideIndex) {
      m_layout = rightL.childAtIndex(nextIndex);
      setPositionAtEndpointAfterHorizontalMove(OMG::HorizontalDirection::Right);
      return true;
    }
  }

  Layout leftL = leftLayout();
  if (!leftL.isUninitialized()) {
    nextIndex = leftL.indexOfNextChildToPointToAfterVerticalCursorMove(direction, LayoutNode::k_outsideIndex, LayoutNode::PositionInLayout::Right);
    if (nextIndex != LayoutNode::k_outsideIndex) {
      m_layout = leftL.childAtIndex(nextIndex);
      setPositionAtEndpointAfterHorizontalMove(OMG::HorizontalDirection::Left);
      return true;
    }
  }

  /* Step 2:
   * Ask ancestor if cursor can move vertically. */
  Layout p = m_layout.parent();
  Layout currentChild = m_layout;
  LayoutNode::PositionInLayout currentPosition = m_position == 0 ? LayoutNode::PositionInLayout::Left : ((m_layout.isHorizontal() && m_position == m_layout.numberOfChildren()) || (!m_layout.isHorizontal() && m_position == 1) ? LayoutNode::PositionInLayout::Right : LayoutNode::PositionInLayout::Middle);
  while (!p.isUninitialized()) {
    int childIndex = p.indexOfChild(currentChild);
    int nextIndex = p.indexOfNextChildToPointToAfterVerticalCursorMove(direction, childIndex, currentPosition);
    if (nextIndex != LayoutNode::k_outsideIndex) {
      m_layout = p.childAtIndex(nextIndex);
      // TODO: Replace wit score in descendant
      setPositionAtEndpointAfterHorizontalMove(OMG::HorizontalDirection::Right);
      return true;
    }
    currentChild = p;
    p = p.parent();
    currentPosition = LayoutNode::PositionInLayout::Middle;
  }
  return false;
}

void LayoutCursor::privateStartSelecting() {
  m_startOfSelection = m_position;
}

void LayoutCursor::stopSelecting() {
  m_startOfSelection = -1;
}

bool LayoutCursor::setEmptyRectangleVisibility(EmptyRectangle::State state) {
  bool result = false;
  if (m_layout.isHorizontal()) {
    result = static_cast<HorizontalLayout&>(m_layout).setEmptyVisibility(state);
  }
  Layout leftL = leftLayout();
  if (!leftL.isUninitialized() &&
      leftL.type() == LayoutNode::Type::VerticalOffsetLayout &&
      static_cast<VerticalOffsetLayout&>(leftL).horizontalPosition() == VerticalOffsetLayoutNode::HorizontalPosition::Prefix) {
    result = static_cast<VerticalOffsetLayout&>(leftL).setEmptyVisibility(state) || result;
  }
  Layout rightL = rightLayout();
  if (!rightL.isUninitialized() &&
      rightL.type() == LayoutNode::Type::VerticalOffsetLayout &&
      static_cast<VerticalOffsetLayout&>(rightL).horizontalPosition() == VerticalOffsetLayoutNode::HorizontalPosition::Suffix) {
    result = static_cast<VerticalOffsetLayout&>(rightL).setEmptyVisibility(state) || result;
  }
  return result;
}

void LayoutCursor::invalidateSizesAndPositions() {
  Layout layoutToInvalidate = m_layout;
  while (!layoutToInvalidate.parent().isUninitialized()) {
    layoutToInvalidate = layoutToInvalidate.parent();
  }
  layoutToInvalidate.invalidAllSizesPositionsAndBaselines();
}

}

#include <poincare/layout_cursor.h>
#include <poincare/binomial_coefficient_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/curly_brace_layout.h>
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

void LayoutCursor::setPosition(int position) {
  assert(position >= 0);
  assert((m_layout.isHorizontal() && position <= m_layout.numberOfChildren()) || (!m_layout.isHorizontal() && position <= 1));
  assert(!isSelecting());
  LayoutCursor previousCursor = *this;
  m_position = position;
  didEnterCurrentPosition(previousCursor);
}

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
  if (direction == OMG::Direction::Up || direction == OMG::Direction::Down) {
    moved = verticalMove(direction == OMG::Direction::Up ? OMG::VerticalDirection::Up : OMG::VerticalDirection::Down, shouldRedrawLayout);
  } else {
    moved = horizontalMove(direction == OMG::Direction::Left ? OMG::HorizontalDirection::Left : OMG::HorizontalDirection::Right, shouldRedrawLayout);
  }
  assert(!*shouldRedrawLayout || moved);
  if (moved) {
    *shouldRedrawLayout = selecting || *shouldRedrawLayout;
    // Ensure that didEnterCurrentPosition is always called by being left of ||
    *shouldRedrawLayout = didEnterCurrentPosition(cloneCursor) || *shouldRedrawLayout;
  }
  if (*shouldRedrawLayout) {
    invalidateSizesAndPositions();
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

static bool IsEmptyChildOfGridLayout(Layout l) {
  Layout parent = l.parent();
  return l.isEmpty() && !parent.isUninitialized() && GridLayoutNode::IsGridLayoutType(parent.type());
}

static Layout LeftOrRightMostLayout(Layout l, OMG::HorizontalDirection direction) {
  return l.isHorizontal() ? (l.childAtIndex(direction == OMG::HorizontalDirection::Left ? 0 : l.numberOfChildren() - 1) ) : l;
}

static bool IsTemporaryAutocompletedBracketPair(Layout l, AutocompletedBracketPairLayoutNode::Side tempSide) {
  return AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(l.type()) && static_cast<AutocompletedBracketPairLayoutNode *>(l.node())->isTemporary(tempSide);
}

// Return leftParenthesisIndex
static int ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(HorizontalLayout l, int index) {
  int leftParenthesisIndex = index;
  int dummy = 0;
  while (leftParenthesisIndex > 0 && l.childAtIndex(leftParenthesisIndex).isCollapsable(&dummy, true)) {
    leftParenthesisIndex--;
  }
  HorizontalLayout h = HorizontalLayout::Builder();
  int i = index;
  while (i >= leftParenthesisIndex) {
    Layout child = l.childAtIndex(i);
    l.removeChildAtIndexInPlace(i);
    h.addOrMergeChildAtIndex(child, 0);
    i--;
  }
  ParenthesisLayout p = ParenthesisLayout::Builder(h);
  l.addOrMergeChildAtIndex(p, leftParenthesisIndex);
  return leftParenthesisIndex;
}

/* Layout insertion */
void LayoutCursor::insertLayoutAtCursor(Layout layout, Context * context, bool forceRight, bool forceLeft) {
  assert(!isUninitialized() && isValid());
  if (layout.isEmpty()) {
    return;
  }
  // - Step 1 - Delete selection
  deleteAndResetSelection();

  /* - Step 2 - Add empty row to grid layout if needed
   * When an empty child at the bottom or right of the grid is filled,
   * an empty row/column is added below/on the right.
   */
  if (IsEmptyChildOfGridLayout(m_layout)) {
    static_cast<GridLayoutNode *>(m_layout.parent().node())->willFillEmptyChildAtIndex(m_layout.parent().indexOfChild(m_layout));
  }

  /* - Step 3 - Close brackets on the left/right
   *
   * For example, if the current layout is "(3+4]|" (where "|"" is the cursor
   * and "]" is a temporary parenthesis), inserting something on the right
   * should make the parenthesis permanent.
   * "(3+4]|" -> insert "x" -> "(3+4)x|"
   *
   * But if a new parenthesis is inserted, you might not want to make the
   * previous one permanent.
   * "(3+4]|" -> insert "[)+2" -> "(3+4][)+2|"
   * So if the newly inserted one is temporary on its other side, the current
   * bracket is not made permanent here.
   *
   * Later at Step 9, balanceAutocompletedBrackets will make it so:
   * "(3+4][)+2|" -> balance brackets -> "(3+4)+2|"
   * */
  Layout leftL = leftLayout();
  Layout rightL = rightLayout();
  /* Check if the left layout of the inserted layout is a bracket opened on the
   * left.
   * Ex: If layout = "[)+4" -> left layout is a bracket opened on its left
   *     If layout = "(]+4" -> left layout is NOT a bracket opened on is left
   */
  if (!IsTemporaryAutocompletedBracketPair(LeftOrRightMostLayout(layout, OMG::HorizontalDirection::Left), AutocompletedBracketPairLayoutNode::Side::Left) &&
      !leftL.isUninitialized() &&
      AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(leftL.type())) {
    static_cast<AutocompletedBracketPairLayoutNode *>(leftL.node())->makeThisAndChildrenPermanent(AutocompletedBracketPairLayoutNode::Side::Right);
  }
  /* Same than above with right side. */
  if (!IsTemporaryAutocompletedBracketPair(LeftOrRightMostLayout(layout, OMG::HorizontalDirection::Right), AutocompletedBracketPairLayoutNode::Side::Right) &&
      !rightL.isUninitialized() &&
      AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(rightL.type())) {
    static_cast<AutocompletedBracketPairLayoutNode *>(rightL.node())->makeThisAndChildrenPermanent(AutocompletedBracketPairLayoutNode::Side::Left);
  }

  /* - Step 4 - Add parenthesis around vertical offset
   * To avoid ambiguity between a^(b^c) and (a^b)^c when representing a^b^c,
   * add parentheses to make (a^b)^c. */
  if (m_layout.isHorizontal() && layout.type() == LayoutNode::Type::VerticalOffsetLayout && static_cast<VerticalOffsetLayout&>(layout).isSuffixSuperscript()) {
    if (!leftL.isUninitialized() && leftL.type() == LayoutNode::Type::VerticalOffsetLayout && static_cast<VerticalOffsetLayout&>(leftL).isSuffixSuperscript()) {
      // Insert ^c left of a^b -> turn a^b into (a^b)
      int leftParenthesisIndex = ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(static_cast<HorizontalLayout&>(m_layout), m_layout.indexOfChild(leftL));
      m_position = leftParenthesisIndex + 1;
    }

    if (!rightL.isUninitialized() && rightL.type() == LayoutNode::Type::VerticalOffsetLayout && static_cast<VerticalOffsetLayout&>(rightL).isSuffixSuperscript() && m_layout.indexOfChild(rightL) > 0) {
      // Insert ^b right of a in a^c -> turn a^c into (a)^c
      int leftParenthesisIndex = ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(static_cast<HorizontalLayout&>(m_layout), m_layout.indexOfChild(rightL) - 1);
      m_layout = m_layout.childAtIndex(leftParenthesisIndex).childAtIndex(0);
      m_position = m_layout.numberOfChildren();
    }
  }

  // - Step 5 - Find position to point to if layout will me merged
  LayoutCursor previousCursor = *this;
  Layout childToPoint;
  bool layoutToInsertIsHorizontal = layout.isHorizontal();
  if (layoutToInsertIsHorizontal) {
    childToPoint = (forceRight || forceLeft) ? Layout() : static_cast<HorizontalLayout &>(layout).deepChildToPointToWhenInserting();
    if (!childToPoint.isUninitialized() && AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(childToPoint.type())) {
      childToPoint = childToPoint.childAtIndex(0);
    }
  }

  // - Step 6 - Insert layout
  if (m_layout.isHorizontal()) {
    int positionShift = layout.isHorizontal() ? layout.numberOfChildren() : 1;
    static_cast<HorizontalLayout&>(m_layout).addOrMergeChildAtIndex(layout, m_position);
    m_position += forceLeft ? 0 : positionShift;
  } else {
    /* Replace the current layout with an HorizontalLayout so that a sibling
     * can be added to it. */
    assert(m_layout.parent().isUninitialized() || !m_layout.parent().isHorizontal());
    HorizontalLayout newParent = HorizontalLayout::Builder();
    m_layout.replaceWithInPlace(newParent);
    bool insertLeftOfCurrentLayout = m_position == 0;
    newParent.addOrMergeChildAtIndex(m_layout, 0);
    newParent.addOrMergeChildAtIndex(layout, insertLeftOfCurrentLayout ? 0 : newParent.numberOfChildren());
    m_layout = newParent;
    /* How to compute new position:
     * C is the current layout, that was there before insertion.
     * N is the new layout(s), that were inserted.
     *
     * If it's inserted left of the current layout:
     * |C -> NNNNN|C    the new position is numberOfChildren - 1
     * if forceLeft:
     * |C -> |NNNNNC    the new position is 0
     *
     * If it's inserted right of the current layout:
     * C| -> CNNNNN|    the new position is numberOfChildren
     * if forceLeft:
     * C| -> C|NNNNN    the new position is 1
     * */
    m_position = (forceLeft ? 1 : m_layout.numberOfChildren()) - insertLeftOfCurrentLayout;
  }

  /* - Step 7 - Collapse siblings and find position to point to if layout was
   * not merged */
  if (!layoutToInsertIsHorizontal) {
    collapseSiblingsOfLayout(layout);
    int indexOfChildToPointTo = (forceRight || forceLeft) ? LayoutNode::k_outsideIndex : layout.indexOfChildToPointToWhenInserting();
    if (indexOfChildToPointTo != LayoutNode::k_outsideIndex) {
      childToPoint = layout.childAtIndex(indexOfChildToPointTo);
    }
  }

  // - Step 8 - Point to required position
  if (!childToPoint.isUninitialized()) {
    *this = LayoutCursor(childToPoint, OMG::HorizontalDirection::Left);
    didEnterCurrentPosition(previousCursor);
  }

  // - Step 9 - Balance brackets
  balanceAutocompletedBracketsAndKeepAValidCursor();

  // - Step 10 - Invalidate layout sizes and positions
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
  insertLayoutAtCursor(MatrixLayout::EmptyMatrixBuilder(), context);
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
  /* Force the cursor right of the layout. */
  insertLayoutAtCursor(
    VerticalOffsetLayout::Builder(
      CodePointLayout::Builder('2'),
      VerticalOffsetLayoutNode::VerticalPosition::Superscript),
    context, true);
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
  insertLayoutAtCursor(
    FractionLayout::Builder(
      HorizontalLayout::Builder(),
      HorizontalLayout::Builder()),
    context);
}

void LayoutCursor::insertText(const char * text, Context * context, bool forceCursorRightOfText, bool forceCursorLeftOfText) {
  UTF8Decoder decoder(text);

  CodePoint codePoint = decoder.nextCodePoint();
  if (codePoint == UCodePointNull) {
    return;
  }

  /* - Step 1 -
   * Read the text from left to right and create an Horizontal layout
   * containing the layouts corresponding to each code point. */
  HorizontalLayout layoutToInsert = HorizontalLayout::Builder();
  HorizontalLayout currentLayout = layoutToInsert;
  // This is only used to check if we properly left the last subscript
  int currentSubscriptDepth = 0;

  while (codePoint != UCodePointNull) {
    assert(!codePoint.isCombining());
    CodePoint nextCodePoint = decoder.nextCodePoint();
    if (codePoint == UCodePointEmpty) {
      codePoint = nextCodePoint;
      assert(!codePoint.isCombining());
      continue;
    }

    /* TODO: The insertion of subscripts should be replaced with a parser
     * that creates layout. This is a draft of this. */

    /* - Step 1.1 - Handle subscripts
     * Subscripts are serialized as "\x14{...\x14}". When the code points
     * "\x14{" are encountered by the decoder, create a subscript layout
     * and continue insertion in it. When the code points "\x14}" are
     * encountered, leave the subscript and continue the insertion in its
     * parent. */
    if (codePoint == UCodePointSystem) {
      if (nextCodePoint == '{') {
        // Enter a subscript
        Layout newChild = VerticalOffsetLayout::Builder(HorizontalLayout::Builder(), VerticalOffsetLayoutNode::VerticalPosition::Subscript);
        currentSubscriptDepth++;
        nextCodePoint = decoder.nextCodePoint();
        Layout horizontalChildOfSubscript = newChild.childAtIndex(0);
        assert(horizontalChildOfSubscript.isEmpty());
        currentLayout = static_cast<HorizontalLayout&>(horizontalChildOfSubscript);
        codePoint = nextCodePoint;
        continue;
      }
      // UCodePointSystem should be inserted only for system braces
      assert(nextCodePoint == '}' && currentSubscriptDepth > 0);
      // Leave the subscript
      currentSubscriptDepth--;
      Layout subscript = currentLayout;
      while (subscript.type() != LayoutNode::Type::VerticalOffsetLayout) {
        subscript = subscript.parent();
        assert(!subscript.isUninitialized());
      }
      Layout parentOfSubscript = subscript.parent();
      assert(!parentOfSubscript.isUninitialized() && parentOfSubscript.isHorizontal());
      currentLayout = static_cast<HorizontalLayout&>(parentOfSubscript);
      codePoint = decoder.nextCodePoint();
      continue;
    }

    // - Step 1.2 - Handle code points and brackets
    Layout newChild;
    LayoutNode::Type bracketType;
    AutocompletedBracketPairLayoutNode::Side bracketSide;
    if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairCodePoint(codePoint, &bracketType, &bracketSide)) {
      // Brackets will be balanced later in insertLayoutAtCursor
      newChild = AutocompletedBracketPairLayoutNode::BuildFromBracketType(bracketType);
      static_cast<AutocompletedBracketPairLayoutNode *>(newChild.node())->setTemporary(AutocompletedBracketPairLayoutNode::OtherSide(bracketSide), true);
    } else if (nextCodePoint.isCombining()) {
      newChild = CombinedCodePointsLayout::Builder(codePoint, nextCodePoint);
      nextCodePoint = decoder.nextCodePoint();
    } else {
      newChild = CodePointLayout::Builder(codePoint);
    }
    currentLayout.addOrMergeChildAtIndex(newChild, currentLayout.numberOfChildren());
    codePoint = nextCodePoint;
  }
  assert(currentSubscriptDepth == 0);

  // - Step 2 - Inserted the created layout
  insertLayoutAtCursor(layoutToInsert, context, forceCursorRightOfText, forceCursorLeftOfText);

  // TODO: Restore beautification
}

void LayoutCursor::performBackspace() {
  if (isSelecting()) {
    deleteAndResetSelection();
    return;
  }

  LayoutCursor previousCursor = *this;
  Layout leftL = leftLayout();
  if (!leftL.isUninitialized()) {
    LayoutNode::DeletionMethod deletionMethod = leftL.deletionMethodForCursorLeftOfChild(LayoutNode::k_outsideIndex);
    privateDelete(deletionMethod, false);
  } else {
    assert(m_position == leftMostPosition());
    Layout p = m_layout.parent();
    if (p.isUninitialized()) {
      return;
    }
    LayoutNode::DeletionMethod deletionMethod = p.deletionMethodForCursorLeftOfChild(p.indexOfChild(m_layout));
    privateDelete(deletionMethod, true);
  }
  removeEmptyRowOrColumnOfGridParentIfNeeded();
  didEnterCurrentPosition(previousCursor),
  invalidateSizesAndPositions();
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
  removeEmptyRowOrColumnOfGridParentIfNeeded();
  didEnterCurrentPosition();
  invalidateSizesAndPositions();
}

bool LayoutCursor::didEnterCurrentPosition(LayoutCursor previousPosition) {
  bool changed = false;
  if (!previousPosition.isUninitialized() && previousPosition.isValid()) {
    /* Order matters: First show the empty rectangle at position, because when
     * leaving a piecewise operator layout, empty rectangles can be set back
     * to Hidden. */
    changed = previousPosition.setEmptyRectangleVisibilityAtCurrentPosition(EmptyRectangle::State::Visible) || changed;
    changed = previousPosition.layout().deleteGraySquaresBeforeLeavingGrid(m_layout) || changed;
    if (changed) {
      previousPosition.invalidateSizesAndPositions();
    }
  }
  if (isUninitialized()) {
    return changed;
  }
  assert(isValid());
  /* Order matters: First enter the grid, because when entering a piecewise
   * operator layout, empty rectangles can be set back to Visible. */
  changed = m_layout.createGraySquaresAfterEnteringGrid(previousPosition.layout()) || changed;
  changed = setEmptyRectangleVisibilityAtCurrentPosition(EmptyRectangle::State::Hidden) || changed;
  if (changed) {
    invalidateSizesAndPositions();
  }
  return changed;
}

bool LayoutCursor::didExitPosition() {
  LayoutCursor lc;
  return lc.didEnterCurrentPosition(*this);
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

void LayoutCursor::setLayout(Layout l, OMG::HorizontalDirection sideOfLayout) {
  if (!l.isHorizontal() && !l.parent().isUninitialized() && l.parent().isHorizontal()) {
    m_layout = l.parent();
    m_position = m_layout.indexOfChild(l) + (sideOfLayout == OMG::HorizontalDirection::Right);
    return;
  }
  m_layout = l;
  m_position = sideOfLayout == OMG::HorizontalDirection::Left ? leftMostPosition() : rightMostPosition();
}

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

bool LayoutCursor::horizontalMove(OMG::HorizontalDirection direction, bool * shouldRedrawLayout) {
  Layout nextLayout = Layout();
  /* Search the nextLayout on the left/right to ask it where
   * the cursor should go when entering from outside.
   *
   * Example in the layout of 3+4/5 :
   * § is the cursor and -> the direction
   *
   *       4
   * 3+§->---
   *       5
   *
   * Here the cursor must move right but should not "jump" over the fraction,
   * so will ask its rightLayout (the fraction), where it should enter
   * (numerator or denominator).
   *
   * Example in the layout of 12+34:
   * § is the cursor and -> the direction
   *
   * 12+§->34
   *
   * Here the cursor will ask its rightLayout (the "3"), where it should go.
   * This will result in the "3" answering "outside", so that the cursor
   * jumps over it.
   * */
  int currentIndexInNextLayout = LayoutNode::k_outsideIndex;
  if (direction == OMG::HorizontalDirection::Right) {
    nextLayout = rightLayout();
  } else {
    nextLayout = leftLayout();
  }

  if (nextLayout.isUninitialized()) {
    /* If nextLayout is uninitialized, the cursor is at the left-most or
     * right-most position. It should move to the parent.
     *
     * Example in an integral layout:
     * § is the cursor and -> the direction
     *
     *   / 10§->
     *  /
     *  |
     *  |     1+ln(x) dx
     *  |
     *  /
     * / -10
     *
     * Here the cursor must move right but has no rightLayout. So it will
     * ask its parent what it should do when leaving its upper bound child
     * from the right (go to integrand).
     *
     * Example in a square root layout:
     * § is the cursor and -> the direction
     *  _______
     * √1234§->
     *
     * Here the cursor must move right but has no rightLayout. So it will
     * ask its parent what it should do when leaving its only child from
     * from the right (leave the square root).
     * */
    if (m_layout.parent().isUninitialized()) {
      return false;
    }
    nextLayout = m_layout.parent();
    currentIndexInNextLayout = nextLayout.indexOfChild(m_layout);
  }
  assert(!nextLayout.isUninitialized());
  assert(!nextLayout.isHorizontal());

  /* If the cursor is selecting, it should not enter a new layout
   * but select all of it. */
  int newIndex = isSelecting() ? LayoutNode::k_outsideIndex : nextLayout.indexAfterHorizontalCursorMove(direction, currentIndexInNextLayout, shouldRedrawLayout);
  assert(newIndex != LayoutNode::k_cantMoveIndex);

  if (newIndex != LayoutNode::k_outsideIndex) {
    /* Enter the next layout child
     *
     *       4                                        §4
     * 3+§->---          : newIndex = numerator ==> 3+---
     *       5                                         5
     *
     *
     *   / 10§->                                     / 10
     *  /                                           /
     *  |                                           |
     *  |     1+ln(x) dx : newIndex = integrand ==> |     §1+ln(x) dx
     *  |                                           |
     *  /                                           /
     * / -10                                       / -10
     *
     * */
    m_layout = nextLayout.childAtIndex(newIndex);
    m_position = direction == OMG::HorizontalDirection::Right ? leftMostPosition() : rightMostPosition();
    return true;
  }

  /* The new index is outside.
   * If it's not selecting, it can be because there is no child to go into:
   *
   * 12+§->34  : newIndex = outside of the 3    ==> 12+3§4
   *
   *  _______                                        ____ §
   * √1234§->  : newIndex = outside of the sqrt ==> √1234 §
   *
   * If it's selecting, the cursor should always leave the layout and all of
   * it will be selected.
   *
   *   / 10§->                                   / 10          §
   *  /                                         /              §
   *  |                                         |              §
   *  |     1+ln(x) dx : newIndex = outside ==> |   1+ln(x) dx §
   *  |                                         |              §
   *  /                                         /              §
   * / -10                                     / -10           §
   *
   * */
  Layout parent = nextLayout.parent();
  Layout previousLayout = m_layout;
  if (!parent.isUninitialized() && parent.isHorizontal()) {
    m_layout = parent;
    m_position = m_layout.indexOfChild(nextLayout) + (direction == OMG::HorizontalDirection::Right);
  } else {
    m_layout = nextLayout;
    m_position = direction == OMG::HorizontalDirection::Right;
  }

  if (isSelecting() && m_layout != previousLayout) {
    /* If the cursor went into the parent, start the selection before
     * the layout that was just left (or after depending on the direction
     * of the selection). */
    m_startOfSelection = m_position + (direction == OMG::HorizontalDirection::Right ? -1 : 1);
  }
  return true;

}

bool LayoutCursor::verticalMove(OMG::VerticalDirection direction, bool * shouldRedrawLayout) {
  Layout previousLayout = m_layout;
  bool moved = verticalMoveWithoutSelection(direction, shouldRedrawLayout);

  // Handle selection (find a common ancestor to previous and current layout)
  if (moved && isSelecting() && previousLayout != m_layout) {
    TreeHandle commonAncestor = m_layout.commonAncestorWith(previousLayout);
    assert(!commonAncestor.isUninitialized());
    Layout layoutAncestor = static_cast<Layout&>(commonAncestor);
    // Down goes left to right and up goes right to left
    setLayout(layoutAncestor, direction == OMG::VerticalDirection::Up ? OMG::HorizontalDirection::Left : OMG::HorizontalDirection::Right);
    m_startOfSelection = m_position + (direction == OMG::VerticalDirection::Up ? 1 : -1);
  }
  return moved;
}

static void ScoreCursorInDescendants(KDPoint p, Layout l, KDFont::Size font, LayoutCursor * result) {
  KDCoordinate currentDistance = p.squareDistanceTo(result->middleLeftPoint(font));
  /* Put a cursor left and right of l.
   * If l.parent is an HorizontalLayout, just put it left since the right
   * of one child is the left of another one, except if l is the last child.
   * */
  Layout parent = l.parent();
  bool checkOnlyLeft = !parent.isUninitialized() && parent.isHorizontal() && parent.indexOfChild(l) < parent.numberOfChildren() - 1;
  int numberOfDirectionsToCheck = 1 + !checkOnlyLeft;
  for (int i = 0; i < numberOfDirectionsToCheck; i++) {
    LayoutCursor tempCursor = LayoutCursor(l, i == 0 ? OMG::HorizontalDirection::Left : OMG::HorizontalDirection::Right);
    if (currentDistance > p.squareDistanceTo(tempCursor.middleLeftPoint(font))) {
      *result = tempCursor;
    }
  }
  int nChildren = l.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    ScoreCursorInDescendants(p, l.childAtIndex(i), font, result);
  }
}

static LayoutCursor ClosestCursorInDescendantsOfLayout(LayoutCursor currentCursor, Layout l, KDFont::Size font) {
  LayoutCursor result = LayoutCursor(l, OMG::HorizontalDirection::Left);
  ScoreCursorInDescendants(currentCursor.middleLeftPoint(font), l, font, &result);
  return result;
}

bool LayoutCursor::verticalMoveWithoutSelection(OMG::VerticalDirection direction, bool * shouldRedrawLayout) {
  /* Step 1:
   * Try to enter right or left layout if it can be entered through up/down
   * */
  if (!isSelecting()) {
    Layout nextLayout = rightLayout();
    LayoutNode::PositionInLayout positionRelativeToNextLayout = LayoutNode::PositionInLayout::Left;
    // Repeat for right and left
    for (int i = 0; i < 2; i++) {
      if (!nextLayout.isUninitialized()) {
        int nextIndex = nextLayout.indexAfterVerticalCursorMove(direction, LayoutNode::k_outsideIndex,positionRelativeToNextLayout, shouldRedrawLayout);
        if (nextIndex != LayoutNode::k_cantMoveIndex) {
          assert(nextIndex != LayoutNode::k_outsideIndex);
          assert(!nextLayout.isHorizontal());
          m_layout = nextLayout.childAtIndex(nextIndex);
          m_position = positionRelativeToNextLayout == LayoutNode::PositionInLayout::Left ? leftMostPosition() : rightMostPosition();
          return true;
        }
      }
      nextLayout = leftLayout();
      positionRelativeToNextLayout = LayoutNode::PositionInLayout::Right;
    }
  }

  /* Step 2:
   * Ask ancestor if cursor can move vertically. */
  Layout p = m_layout.parent();
  Layout currentChild = m_layout;
  LayoutNode::PositionInLayout currentPosition = m_position == leftMostPosition() ? LayoutNode::PositionInLayout::Left : (m_position == rightMostPosition() ? LayoutNode::PositionInLayout::Right : LayoutNode::PositionInLayout::Middle);
  while (!p.isUninitialized()) {
    int childIndex = p.indexOfChild(currentChild);
    int nextIndex = p.indexAfterVerticalCursorMove(direction, childIndex, currentPosition, shouldRedrawLayout);
    if (nextIndex != LayoutNode::k_cantMoveIndex) {
      if (nextIndex == LayoutNode::k_outsideIndex) {
        assert(currentPosition != LayoutNode::PositionInLayout::Middle);
        setLayout(p, currentPosition == LayoutNode::PositionInLayout::Left ? OMG::HorizontalDirection::Left : OMG::HorizontalDirection::Right);
      } else {
        assert(!p.isHorizontal());
        // We assume the new cursor is the same whatever the font
        LayoutCursor newCursor = ClosestCursorInDescendantsOfLayout(*this, p.childAtIndex(nextIndex), KDFont::Size::Large);
        m_layout = newCursor.layout();
        m_position = newCursor.position();
      }
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

bool LayoutCursor::setEmptyRectangleVisibilityAtCurrentPosition(EmptyRectangle::State state) {
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

void LayoutCursor::privateDelete(LayoutNode::DeletionMethod deletionMethod, bool deletionAppliedToParent) {
  assert(!deletionAppliedToParent || !m_layout.parent().isUninitialized());

  if (deletionMethod == LayoutNode::DeletionMethod::MoveLeft) {
    bool dummy = false;
    move(OMG::Direction::Left, false, &dummy);
    return;
  }

  if (deletionMethod == LayoutNode::DeletionMethod::DeleteParent) {
    assert(deletionAppliedToParent);
    Layout p = m_layout.parent();
    assert(!p.isUninitialized() && !p.isHorizontal());
    Layout parentOfP = p.parent();
    if (parentOfP.isUninitialized() || !parentOfP.isHorizontal()) {
      assert(m_position == 0);
      p.replaceWithInPlace(m_layout);
    } else {
      m_position = parentOfP.indexOfChild(p);
      static_cast<HorizontalLayout&>(parentOfP).removeChildAtIndexInPlace(m_position);
      static_cast<HorizontalLayout&>(parentOfP).addOrMergeChildAtIndex(m_layout, m_position);
      m_layout = parentOfP;
    }
    return;
  }

  if (deletionMethod == LayoutNode::DeletionMethod::AutocompletedBracketPairMakeTemporary) {
    if (deletionAppliedToParent) { // Inside bracket
      Layout parent = m_layout.parent();
      assert(AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(parent.type()));
      static_cast<AutocompletedBracketPairLayoutNode *>(parent.node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Left, true);
    } else { // Right of bracket
      assert(AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(leftLayout().type()));
      static_cast<AutocompletedBracketPairLayoutNode *>(leftLayout().node())->setTemporary(AutocompletedBracketPairLayoutNode::Side::Right, true);
    }
    bool dummy = false;
    move(OMG::Direction::Left, false, &dummy);
    balanceAutocompletedBracketsAndKeepAValidCursor();
    return;
  }

  if (deletionMethod == LayoutNode::DeletionMethod::FractionDenominatorDeletion) {
    // Merge the numerator and denominator and replace the fraction with it
    assert(deletionAppliedToParent);
    Layout fraction = m_layout.parent();
    assert(fraction.type() == LayoutNode::Type::FractionLayout && fraction.childAtIndex(1) == m_layout);
    Layout numerator = fraction.childAtIndex(0);
    if (!numerator.isHorizontal()) {
      HorizontalLayout hLayout = HorizontalLayout::Builder();
      numerator.replaceWithInPlace(hLayout);
      hLayout.addOrMergeChildAtIndex(numerator, 0);
      numerator = hLayout;
    }
    assert(numerator.isHorizontal());
    m_position = numerator.numberOfChildren();
    static_cast<HorizontalLayout&>(numerator).addOrMergeChildAtIndex(m_layout, numerator.numberOfChildren());
    Layout parentOfFraction = fraction.parent();
    if (parentOfFraction.isUninitialized() || !parentOfFraction.isHorizontal()) {
      assert(m_position == 0);
      fraction.replaceWithInPlace(numerator);
      m_layout = numerator;
    } else {
      int indexOfFraction = parentOfFraction.indexOfChild(fraction);
      m_position += indexOfFraction;
      static_cast<HorizontalLayout&>(parentOfFraction).removeChildAtIndexInPlace(indexOfFraction);
      static_cast<HorizontalLayout&>(parentOfFraction).addOrMergeChildAtIndex(numerator, indexOfFraction);
      m_layout = parentOfFraction;
    }
    return;
  }

  if (deletionMethod == LayoutNode::DeletionMethod::BinomialCoefficientMoveFromKtoN || deletionMethod == LayoutNode::DeletionMethod::GridLayoutMoveToUpperRow) {
    assert(deletionAppliedToParent);
    int newIndex = -1;
    if (deletionMethod == LayoutNode::DeletionMethod::BinomialCoefficientMoveFromKtoN) {
      assert(m_layout.parent().type() == LayoutNode::Type::BinomialCoefficientLayout);
      newIndex = BinomialCoefficientLayoutNode::k_nLayoutIndex;
    } else {
      assert(deletionMethod == LayoutNode::DeletionMethod::GridLayoutMoveToUpperRow);
      assert(GridLayoutNode::IsGridLayoutType(m_layout.parent().type()));
      GridLayoutNode * gridNode = static_cast<GridLayoutNode *>(m_layout.parent().node());
      int currentIndex = m_layout.parent().indexOfChild(m_layout);
      int currentRow = gridNode->rowAtChildIndex(currentIndex);
      assert(currentRow > 0 && gridNode->numberOfColumns() >= 2);
      // - 2 because we want to go the the rightmost column that is not gray
      newIndex = gridNode->indexAtRowColumn(currentRow - 1, gridNode->numberOfColumns() - 2);
    }
    m_layout = m_layout.parent().childAtIndex(newIndex);
    m_position = rightMostPosition();
    return;
  }

  if (deletionMethod == LayoutNode::DeletionMethod::GridLayoutDeleteColumn ||
      deletionMethod == LayoutNode::DeletionMethod::GridLayoutDeleteRow ||
      deletionMethod == LayoutNode::DeletionMethod::GridLayoutDeleteColumnAndRow)
  {
    assert(deletionAppliedToParent);
    assert(GridLayoutNode::IsGridLayoutType(m_layout.parent().type()));
    GridLayoutNode * gridNode = static_cast<GridLayoutNode *>(m_layout.parent().node());
    int currentIndex = m_layout.parent().indexOfChild(m_layout);
    int currentRow = gridNode->rowAtChildIndex(currentIndex);
    int currentColumn = gridNode->columnAtChildIndex(currentIndex);
    if (deletionMethod != LayoutNode::DeletionMethod::GridLayoutDeleteColumn) {
      gridNode->deleteRowAtIndex(currentRow);
    }
    if (deletionMethod != LayoutNode::DeletionMethod::GridLayoutDeleteRow) {
      gridNode->deleteColumnAtIndex(currentColumn);
    }
    int newChildIndex = gridNode->indexAtRowColumn(currentRow, currentColumn);
    *this = LayoutCursor(Layout(gridNode).childAtIndex(newChildIndex));
    didEnterCurrentPosition();
    return;
  }

  assert(deletionMethod == LayoutNode::DeletionMethod::DeleteLayout);
  if (deletionAppliedToParent) {
    setLayout(m_layout.parent(), OMG::HorizontalDirection::Right);
  }
  if (!m_layout.isHorizontal()) {
    assert(m_layout.parent().isUninitialized() || !m_layout.parent().isHorizontal());
    HorizontalLayout hLayout = HorizontalLayout::Builder();
    m_layout.replaceWithInPlace(hLayout);
    hLayout.addOrMergeChildAtIndex(m_layout, 0);
    m_layout = hLayout;
  }
  assert(m_position != 0);
  assert(m_layout.isHorizontal());
  static_cast<HorizontalLayout&>(m_layout).removeChildAtIndexInPlace(m_position - 1);
  m_position--;
}

void LayoutCursor::removeEmptyRowOrColumnOfGridParentIfNeeded() {
  if (!IsEmptyChildOfGridLayout(m_layout)) {
    return;
  }
  Layout parentGrid = m_layout.parent();
  GridLayoutNode * gridNode = static_cast<GridLayoutNode *>(parentGrid.node());
  int currentChildIndex = parentGrid.indexOfChild(m_layout);
  int currentRow = gridNode->rowAtChildIndex(currentChildIndex);
  int currentColumn = gridNode->columnAtChildIndex(currentChildIndex);
  bool changed = gridNode->removeEmptyRowOrColumnAtChildIndexIfNeeded(currentChildIndex);
  if (changed) {
    int newChildIndex = gridNode->indexAtRowColumn(currentRow, currentColumn);
    assert(parentGrid.numberOfChildren() > newChildIndex);
    *this = LayoutCursor(parentGrid.childAtIndex(newChildIndex));
    didEnterCurrentPosition();
  }
}

void LayoutCursor::collapseSiblingsOfLayout(Layout l) {
  if (l.shouldCollapseSiblingsOnRight()) {
    collapseSiblingsOfLayoutOnDirection(l, OMG::HorizontalDirection::Right, l.rightCollapsingAbsorbingChildIndex());
  }
  if (l.shouldCollapseSiblingsOnLeft()) {
    collapseSiblingsOfLayoutOnDirection(l, OMG::HorizontalDirection::Left, l.leftCollapsingAbsorbingChildIndex());
  }
}

void LayoutCursor::collapseSiblingsOfLayoutOnDirection(Layout l, OMG::HorizontalDirection direction, int absorbingChildIndex) {
  /* This method absorbs the siblings of a layout when it's inserted.
   *
   * Example:
   * When inserting √() was just inserted in "1 + √()45 + 3 ",
   * the square root should absorb the 45 and this will output
   * "1 + √(45) + 3"
   *
   * Here l = √(), and absorbingChildIndex = 0 (the inside of the sqrt)
   * */
  Layout absorbingChild = l.childAtIndex(absorbingChildIndex);
  if (absorbingChild.isUninitialized() || !absorbingChild.isEmpty()) {
    return;
  }
  Layout p = l.parent();
  if (p.isUninitialized() || !p.isHorizontal()) {
    return;
  }
  int idxInParent = p.indexOfChild(l);
  int numberOfSiblings = p.numberOfChildren();
  int numberOfOpenParenthesis = 0;

  assert(absorbingChild.isHorizontal()); // Empty is always horizontal
  HorizontalLayout horizontalAbsorbingChild = static_cast<HorizontalLayout&>(absorbingChild);
  HorizontalLayout horizontalParent = static_cast<HorizontalLayout&>(p);
  Layout sibling;
  int step = direction == OMG::HorizontalDirection::Right ? 1 : - 1;
  /* Loop through the siblings and add them into l until an uncollapsable
   * layout is encountered. */
  while (true) {
    if (idxInParent == (direction == OMG::HorizontalDirection::Right ? numberOfSiblings - 1 : 0)) {
      break;
    }
    int siblingIndex = idxInParent + step;
    sibling = horizontalParent.childAtIndex(siblingIndex);
    if (!sibling.isCollapsable(&numberOfOpenParenthesis, direction == OMG::HorizontalDirection::Left)) {
      break;
    }
    horizontalParent.removeChildAtIndexInPlace(siblingIndex);
    int newIndex = direction == OMG::HorizontalDirection::Right ? absorbingChild.numberOfChildren() : 0;
    assert(!sibling.isHorizontal());
    horizontalAbsorbingChild.addOrMergeChildAtIndex(sibling, newIndex);
    numberOfSiblings--;
    if (direction == OMG::HorizontalDirection::Left) {
      idxInParent--;
    }
  }
}

void LayoutCursor::balanceAutocompletedBracketsAndKeepAValidCursor() {
  if (!m_layout.isHorizontal()) {
    return;
  }
  /* Find the top horizontal layout for balancing brackets.
   *
   * This might go again through already balanced brackets but it's safer
   * in order to ensure that all brackets are always balanced after an
   * insertion or a deletion.
   *
   * Stop if the parent of the currentLayout is not horizontal neither
   * a bracket.
   * Ex: When balancing the brackets inside the numerator of a fraction,
   * it's useless to take the parent horizontal layout of the fraction, since
   * brackets outside of the fraction won't impact the ones inside the fraction
   * */
  Layout currentLayout = m_layout;
  Layout currentParent = currentLayout.parent();
  while (!currentParent.isUninitialized() && (currentParent.isHorizontal() || AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(currentParent.type()))) {
    currentLayout = currentParent;
    currentParent = currentLayout.parent();
  }
  // A bracket should always have an horizontal parent
  assert(currentLayout.isHorizontal());
  HorizontalLayout topHorizontalLayout = static_cast<HorizontalLayout&>(currentLayout);
  AutocompletedBracketPairLayoutNode::BalanceBrackets(topHorizontalLayout, static_cast<HorizontalLayout *>(&m_layout), &m_position);
}

}

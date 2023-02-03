#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_selection.h>
//#include <poincare/matrix_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/display.h>

namespace Poincare {

bool LayoutNode::isIdenticalTo(Layout l, bool makeEditable) {
  if (makeEditable) {
    return Layout(this).clone().makeEditable().isIdenticalTo(l.clone().makeEditable(), false);
  }
  if (l.isUninitialized() || type() != l.type()) {
    return false;
  }
  if (identifier() == l.identifier()) {
    return true;
  }
  return protectedIsIdenticalTo(l);
}

// Rendering

void LayoutNode::draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, LayoutSelection selection, KDColor selectionColor) {
  if (!selection.isEmpty() && selection.containsNode(this)) {
    selection = LayoutSelection();
    backgroundColor = selectionColor;
  }

  KDPoint renderingAbsoluteOrigin = absoluteOrigin(font).translatedBy(p);
  KDPoint renderingOriginWithMargin = absoluteOriginWithMargin(font).translatedBy(p);
  KDSize size = layoutSize(font);

  if (size.height() <= 0 || size.width() <= 0
      || size.height() > KDCOORDINATE_MAX - renderingAbsoluteOrigin.y()
      || size.width() > KDCOORDINATE_MAX - renderingAbsoluteOrigin.x()) {
    // Layout size overflows KDCoordinate
    return;
  }

  ctx->fillRect(KDRect(renderingOriginWithMargin, size), backgroundColor);
  if (!selection.isEmpty() && selection.layout().node() == this && isHorizontal()) {
    KDRect selectionRectangle = static_cast<HorizontalLayoutNode *>(this)->relativeSelectionRect(selection.leftPosition(), selection.rightPosition(), font);
    ctx->fillRect(selectionRectangle.translatedBy(renderingOriginWithMargin), selectionColor);
  }
  render(ctx, renderingAbsoluteOrigin, font, expressionColor, backgroundColor);
  for (LayoutNode * l : children()) {
    l->draw(ctx, p, font, expressionColor, backgroundColor, selection, selectionColor);
  }
}

KDPoint LayoutNode::absoluteOriginWithMargin(KDFont::Size font) {
  LayoutNode * p = parent();
  if (!m_flags.m_positioned || m_flags.m_positionFontSize != font) {
    if (p != nullptr) {
      m_frame.setOrigin(p->absoluteOrigin(font).translatedBy(p->positionOfChild(this, font)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_flags.m_positioned = true;
    m_flags.m_positionFontSize = font;
  }
  return m_frame.origin();
}

KDSize LayoutNode::layoutSize(KDFont::Size font) {
  if (!m_flags.m_sized || m_flags.m_sizeFontSize != font) {
    KDSize size = computeSize(font);
    m_frame.setSize(KDSize(size.width() + leftMargin(), size.height()));
    m_flags.m_sized = true;
    m_flags.m_sizeFontSize = font;
  }
  return m_frame.size();
}

KDCoordinate LayoutNode::baseline(KDFont::Size font) {
  if (!m_flags.m_baselined || m_flags.m_baselineFontSize != font) {
    m_baseline = computeBaseline(font);
    m_flags.m_baselined = true;
    m_flags.m_baselineFontSize = font;
  }
  return m_baseline;
}

void LayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_flags.m_sized = false;
  m_flags.m_positioned = false;
  m_flags.m_baselined = false;
  for (LayoutNode * l : children()) {
    l->invalidAllSizesPositionsAndBaselines();
  }
}

int LayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex) const {
  int nChildren = numberOfChildren();
  if (nChildren == 0) {
    assert(currentIndex == k_outsideIndex);
    return k_outsideIndex;
  }
  if (nChildren == 1) {
    assert(currentIndex == k_outsideIndex || currentIndex == 0);
    return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
  }
  assert(false);
  return -2;
}

int LayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex) const {
  assert(currentIndex < numberOfChildren());
  assert(currentIndex != k_outsideIndex || positionAtCurrentIndex != PositionInLayout::Middle);
  return k_outsideIndex;
}

/*
void LayoutNode::askParentToMoveCursorHorizontally(OMG::HorizontalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert((direction.isLeft()  && cursor->position() == LayoutCursor::Position::Left)
      || (direction.isRight() && cursor->position() == LayoutCursor::Position::Right));
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    if (direction.isLeft()) {
      return parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
    }
    assert(direction.isRight());
    return parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

// Tree modification

void LayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  int indexOfPointedLayout = indexOfChild(cursor->layoutNode());
  if (indexOfPointedLayout >= 0) {
    // Case: The pointed layout is a child. Move Left.
    assert(cursor->position() == LayoutCursor::Position::Left);
    bool shouldRecomputeLayout = false;
    cursor->move(OMG::NewDirection::Left(), &shouldRecomputeLayout);
    return;
  }
  assert(cursor->layoutNode() == this);
  LayoutNode * p = parent();
  // Case: this is the pointed layout.
  if (p == nullptr) {
    // Case: No parent. Return.
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Ask the parent.
    p->deleteBeforeCursor(cursor);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Delete the layout
  Layout(p).removeChild(Layout(this), cursor);
  // WARNING: Do no use "this" afterwards
}

bool LayoutNode::deleteBeforeCursorForLayoutContainingArgument(LayoutNode * argumentNode, LayoutCursor * cursor) {
  if (argumentNode && cursor->isEquivalentTo(LayoutCursor(argumentNode, LayoutCursor::Position::Left))) {
    // Case: Left of the argument. Delete the layout, keep the argument.
    Layout thisRef = Layout(this);
    Layout argument = Layout(argumentNode);
    // WARNING: Do not use "this" afterwards
    thisRef.replaceWith(argument, cursor);
    return true;
  }
  if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
    // Case: Right of layout, enter inside layout
    bool temp;
    moveCursorLeft(cursor, &temp);
    return true;
  }
  return false;
}
 */

LayoutNode * LayoutNode::layoutToPointWhenInserting(Expression * correspondingExpression, bool * forceCursorLeftOfText) {
  assert(correspondingExpression != nullptr);
  return numberOfChildren() > 0 ? childAtIndex(0) : this;
}

bool LayoutNode::removeGraySquaresFromAllGridAncestors() {
  bool result = false;
  changeGraySquaresOfAllGridRelatives(false, true, &result);
  return result;
}

bool LayoutNode::removeGraySquaresFromAllGridChildren() {
  bool result = false;
  changeGraySquaresOfAllGridRelatives(false, false, &result);
  return result;
}

bool LayoutNode::addGraySquaresToAllGridAncestors() {
  bool result = false;
  changeGraySquaresOfAllGridRelatives(true, true, &result);
  return result;
}

Layout LayoutNode::makeEditable() {
  /* We visit children if reverse order to avoid visiting the codepoints they
   * might have inserted after them. */
  for (int i = numberOfChildren() - 1; i >= 0; i--) {
    childAtIndex(i)->makeEditable();
  }
  return Layout(this);
}

// Other
bool LayoutNode::canBeOmittedMultiplicationLeftFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true too. If isCollapsable changes, it might not be the
   * case anymore so make sure to modify this function if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, true);
}

bool LayoutNode::canBeOmittedMultiplicationRightFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true and isVerticalOffset is false. If one of these
   * functions changes, it might not be the case anymore so make sure to modify
   * canBeOmittedMultiplicationRightFactor if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, false);
}

Layout LayoutNode::XNTLayout(int childIndex) const {
  LayoutNode * p = parent();
  return p == nullptr ? Layout() : p->XNTLayout(p->indexOfChild(this));
}

// Protected and private

bool LayoutNode::protectedIsIdenticalTo(Layout l) {
  if (numberOfChildren() != l.numberOfChildren()) {
    return false;
  }
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    if (!childAtIndex(i)->isIdenticalTo(l.childAtIndex(i))) {
      return false;
    }
  }
  return true;
}
/*
void LayoutNode::moveCursorVertically(OMG::VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (!equivalentPositionVisited) {
    LayoutCursor cursorEquivalent = equivalentCursor(cursor);
    if (cursorEquivalent.isDefined()) {
      cursor->setLayout(cursorEquivalent.layout());
      cursor->setPosition(cursorEquivalent.position());
      if (direction.isUp()) {
        cursor->layoutNode()->moveCursorUp(cursor, shouldRecomputeLayout, true, forSelection);
      } else {
        cursor->layoutNode()->moveCursorDown(cursor, shouldRecomputeLayout, true, forSelection);
      }
      return;
    }
  }
  LayoutNode * p = parent();
  if (p == nullptr) {
    cursor->setLayout(Layout());
    return;
  }
  if (direction.isUp()) {
    p->moveCursorUp(cursor, shouldRecomputeLayout, true, forSelection);
  } else {
    p->moveCursorDown(cursor, shouldRecomputeLayout, true, forSelection);
  }
}

void LayoutNode::moveCursorInDescendantsVertically(OMG::NewVerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  LayoutNode * childResult = nullptr;
  LayoutNode ** childResultPtr = &childResult;
  LayoutCursor::Position resultPosition = LayoutCursor::Position::Left;
  /* The distance between the cursor and its next position cannot be greater
   * than this initial value of score. *
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  scoreCursorInDescendantsVertically(direction, cursor, shouldRecomputeLayout, childResultPtr, &resultPosition, &resultScore, forSelection);

  // If there is a valid result
  Layout resultRef(childResult);
  if ((*childResultPtr) != nullptr) {
    *shouldRecomputeLayout |= childResult->addGraySquaresToAllGridAncestors();
    // WARNING: Do not use "this" afterwards
  }
  cursor->setLayout(resultRef);
  cursor->setPosition(resultPosition);
}

void LayoutNode::scoreCursorInDescendantsVertically (
    OMG::NewVerticalDirection direction,
    LayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    LayoutNode ** childResult,
    void * resultPosition,
    int * resultScore,
    bool forSelection)
{
  LayoutCursor::Position * castedResultPosition = static_cast<LayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction.isUp() ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
  bool layoutContains = m_frame.contains(cursorMiddleLeft);

  if (layoutIsUnderOrAbove) {
    // Check the distance to a Left cursor.
    int currentDistance = LayoutCursor(this, LayoutCursor::Position::Left).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance <= *resultScore ){
      *childResult = this;
      *castedResultPosition = LayoutCursor::Position::Left;
      *resultScore = currentDistance;
    }

    // Check the distance to a Right cursor.
    currentDistance = LayoutCursor(this, LayoutCursor::Position::Right).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance < *resultScore) {
      *childResult = this;
      *castedResultPosition = LayoutCursor::Position::Right;
      *resultScore = currentDistance;
    }
  }
  if (layoutIsUnderOrAbove || layoutContains) {
    for (LayoutNode * c : children()) {
      c->scoreCursorInDescendantsVertically(direction, cursor, shouldRecomputeLayout, childResult, castedResultPosition, resultScore, forSelection);
    }
  }
}
*/
bool addRemoveGraySquaresInLayoutIfNeeded(bool add, Layout * l) {
  /*if (!GridLayoutNode::IsGridLayoutType(l->type())) {
    return false;
  }
  if (add) {
    static_cast<GridLayoutNode *>(l->node())->startEditing();
  } else {
    static_cast<GridLayoutNode *>(l->node())->stopEditing();
  }
  return true;*/
  return false;
}

void LayoutNode::changeGraySquaresOfAllGridRelatives(bool add, bool ancestors, bool * changedSquares) {
  if (!ancestors) {
    // If in children, we also change the squares for this
    {
      Layout thisLayout = Layout(this);
      if (addRemoveGraySquaresInLayoutIfNeeded(add, &thisLayout)) {
        *changedSquares = true;
      }
    }
    int childrenNumber = numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      /* We cannot use "for l : children()", as the node addresses might change,
       * especially the iterator stopping address. */
      childAtIndex(i)->changeGraySquaresOfAllGridRelatives(add, false, changedSquares);
    }
  } else {
    Layout currentAncestor = Layout(parent());
    while (!currentAncestor.isUninitialized()) {
      if (addRemoveGraySquaresInLayoutIfNeeded(add, &currentAncestor)) {
        *changedSquares = true;
      }
      currentAncestor = currentAncestor.parent();
    }
  }
}

}

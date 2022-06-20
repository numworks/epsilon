#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/matrix_layout.h>
#include <ion/display.h>

namespace Poincare {

bool LayoutNode::isIdenticalTo(Layout l) {
  if (l.isUninitialized() || type() != l.type()) {
    return false;
  }
  if (identifier() == l.identifier()) {
    return true;
  }
  return protectedIsIdenticalTo(l);
}

// Rendering

void LayoutNode::draw(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  bool isSelected = selectionStart != nullptr && selectionEnd != nullptr
    && !selectionStart->isUninitialized() && !selectionStart->isUninitialized()
    && reinterpret_cast<char *>(this) >= reinterpret_cast<char *>(selectionStart->node())
    && reinterpret_cast<char *>(this) <= reinterpret_cast<char *>(selectionEnd->node());
  KDColor backColor = isSelected ? selectionColor : backgroundColor;
  KDPoint renderingAbsoluteOrigin = absoluteOrigin().translatedBy(p);
  ctx->fillRect(KDRect(renderingAbsoluteOrigin, layoutSize()), backColor);
  render(ctx, renderingAbsoluteOrigin, expressionColor, backColor, selectionStart, selectionEnd, selectionColor);
  if (!isSelected) {
    for (LayoutNode * l : children()) {
      l->draw(ctx, p, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
    }
  } else {
    for (LayoutNode * l : children()) {
      l->draw(ctx, p, expressionColor, selectionColor);
    }
  }
}

KDPoint LayoutNode::absoluteOrigin() {
  LayoutNode * p = parent();
  if (!m_positioned) {
    if (p != nullptr) {
      m_frame.setOrigin(p->absoluteOrigin().translatedBy(p->positionOfChild(this)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_positioned = true;
  }
  return m_frame.origin().translatedBy(KDPoint(leftMargin(), 0));;
}

KDSize LayoutNode::layoutSize() {
  if (!m_sized) {
    KDSize size = computeSize();
    m_frame.setSize(KDSize(size.width() + leftMargin(), size.height()));
    m_sized = true;
  }
  return m_frame.size();
}

KDCoordinate LayoutNode::baseline() {
  if (!m_baselined) {
    m_baseline = computeBaseline();
    m_baselined = true;
  }
  return m_baseline;
}

void LayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
  for (LayoutNode * l : children()) {
    l->invalidAllSizesPositionsAndBaselines();
  }
}

// Tree navigation
LayoutCursor LayoutNode::equivalentCursor(LayoutCursor * cursor) {
  // Only HorizontalLayout may have no parent, and it overloads this method
  assert(parent() != nullptr);
  return (cursor->layout().node() == this) ? parent()->equivalentCursor(cursor) : LayoutCursor();
}

// Tree modification

void LayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  int indexOfPointedLayout = indexOfChild(cursor->layoutNode());
  if (indexOfPointedLayout >= 0) {
    // Case: The pointed layout is a child. Move Left.
    assert(cursor->position() == LayoutCursor::Position::Left);
    bool shouldRecomputeLayout = false;
    cursor->moveLeft(&shouldRecomputeLayout);
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
  // Case: Right. Delete the layout (or replace it with an EmptyLayout).
  Layout(p).removeChild(Layout(this), cursor);
  // WARNING: Do no use "this" afterwards
}

LayoutNode * LayoutNode::layoutToPointWhenInserting(Expression * correspondingExpression) {
  assert(correspondingExpression != nullptr);
  return numberOfChildren() > 0 ? childAtIndex(0) : this;
}

bool LayoutNode::removeGraySquaresFromAllMatrixAncestors() {
  bool result = false;
  changeGraySquaresOfAllMatrixRelatives(false, true, &result);
  return result;
}

bool LayoutNode::removeGraySquaresFromAllMatrixChildren() {
  bool result = false;
  changeGraySquaresOfAllMatrixRelatives(false, false, &result);
  return result;
}

bool LayoutNode::addGraySquaresToAllMatrixAncestors() {
  bool result = false;
  changeGraySquaresOfAllMatrixRelatives(true, true, &result);
  return result;
}

bool LayoutNode::willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) {
  if (!force) {
    Layout(this).replaceChildWithEmpty(Layout(l), cursor);
    return false;
  }
  return true;
}

Layout LayoutNode::makeEditable() {
  int i = 0;
  while (i < numberOfChildren()) {
    childAtIndex(i)->makeEditable();
    i++;
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
  for (int i = 0; i < numberOfChildren(); i++) {
    if (!childAtIndex(i)->isIdenticalTo(l.childAtIndex(i))) {
      return false;
    }
  }
  return true;
}

void LayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (!equivalentPositionVisited) {
    LayoutCursor cursorEquivalent = equivalentCursor(cursor);
    if (cursorEquivalent.isDefined()) {
      cursor->setLayout(cursorEquivalent.layout());
      cursor->setPosition(cursorEquivalent.position());
      if (direction == VerticalDirection::Up) {
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
  if (direction == VerticalDirection::Up) {
    p->moveCursorUp(cursor, shouldRecomputeLayout, true, forSelection);
  } else {
    p->moveCursorDown(cursor, shouldRecomputeLayout, true, forSelection);
  }
}

void LayoutNode::moveCursorInDescendantsVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  LayoutNode * childResult = nullptr;
  LayoutNode ** childResultPtr = &childResult;
  LayoutCursor::Position resultPosition = LayoutCursor::Position::Left;
  /* The distance between the cursor and its next position cannot be greater
   * than this initial value of score. */
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  scoreCursorInDescendantsVertically(direction, cursor, shouldRecomputeLayout, childResultPtr, &resultPosition, &resultScore, forSelection);

  // If there is a valid result
  Layout resultRef(childResult);
  if ((*childResultPtr) != nullptr) {
    *shouldRecomputeLayout |= childResult->addGraySquaresToAllMatrixAncestors();
    // WARNING: Do not use "this" afterwards
  }
  cursor->setLayout(resultRef);
  cursor->setPosition(resultPosition);
}

void LayoutNode::scoreCursorInDescendantsVertically (
    VerticalDirection direction,
    LayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    LayoutNode ** childResult,
    void * resultPosition,
    int * resultScore,
    bool forSelection)
{
  LayoutCursor::Position * castedResultPosition = static_cast<LayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction == VerticalDirection::Up ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
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

bool addRemoveGraySquaresInLayoutIfNeeded(bool add, Layout * l) {
  if (l->type() != LayoutNode::Type::MatrixLayout) {
    return false;
  }
  if (add) {
    static_cast<MatrixLayoutNode *>(l->node())->addGraySquares();
  } else {
    static_cast<MatrixLayoutNode *>(l->node())->removeGraySquares();
  }
  return true;
}

void LayoutNode::changeGraySquaresOfAllMatrixRelatives(bool add, bool ancestors, bool * changedSquares) {
  if (!ancestors) {
    // If in children, we also change the squares for this
    {
      Layout thisLayout = Layout(this);
      if (addRemoveGraySquaresInLayoutIfNeeded(add, &thisLayout)) {
        *changedSquares = true;
      }
    }
    for (int i = 0; i < numberOfChildren(); i++) {
      /* We cannot use "for l : children()", as the node addresses might change,
       * especially the iterator stopping address. */
      childAtIndex(i)->changeGraySquaresOfAllMatrixRelatives(add, false, changedSquares);
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

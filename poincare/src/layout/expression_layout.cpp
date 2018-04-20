#include <poincare/expression_layout.h>
#include "empty_layout.h"
#include "horizontal_layout.h"
#include "matrix_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>
#include <ion/display.h>

namespace Poincare {

ExpressionLayout::ExpressionLayout() :
  m_parent(nullptr),
  m_baseline(0),
  m_sized(false),
  m_baselined(false),
  m_positioned(false),
  m_frame(KDRectZero)
{
}

void ExpressionLayout::draw(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  int i = 0;
  while (ExpressionLayout * c = editableChild(i++)) {
    c->draw(ctx, p, expressionColor, backgroundColor);
  }
  render(ctx, absoluteOrigin().translatedBy(p), expressionColor, backgroundColor);
}

KDPoint ExpressionLayout::origin() {
  if (m_parent == nullptr) {
    return absoluteOrigin();
  } else {
    return KDPoint(absoluteOrigin().x() - m_parent->absoluteOrigin().x(),
        absoluteOrigin().y() - m_parent->absoluteOrigin().y());
  }
}

KDPoint ExpressionLayout::absoluteOrigin() {
  if (!m_positioned) {
    if (m_parent != nullptr) {
      m_frame.setOrigin(m_parent->absoluteOrigin().translatedBy(m_parent->positionOfChild(this)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_positioned = true;
  }
  return m_frame.origin();
}

KDSize ExpressionLayout::size() {
  if (!m_sized) {
    m_frame.setSize(computeSize());
    m_sized = true;
  }
  return m_frame.size();
}

KDCoordinate ExpressionLayout::baseline() {
  if (!m_baselined) {
    computeBaseline();
    m_baselined = true;
  }
  return m_baseline;
}

void ExpressionLayout::invalidAllSizesPositionsAndBaselines() {
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
  for (int i = 0; i < numberOfChildren(); i++) {
    editableChild(i)->invalidAllSizesPositionsAndBaselines();
  }
}

const ExpressionLayout * ExpressionLayout::child(int i) const {
  assert(i >= 0);
  if (i < numberOfChildren()) {
    assert(children()[i]->parent() == nullptr || children()[i]->parent() == this);
    return children()[i];
  }
  return nullptr;
}

int ExpressionLayout::indexOfChild(const ExpressionLayout * child) const {
  if (child == nullptr) {
    return -1;
  }
  for (int i = 0; i < numberOfChildren(); i++) {
    if (children()[i] == child) {
      return i;
    }
  }
  return -1;
}

bool ExpressionLayout::hasAncestor(const ExpressionLayout * e, bool includeSelf) const {
  if (includeSelf && e == this) {
    return true;
  }
  if (m_parent == e) {
    return true;
  }
  if (m_parent == nullptr) {
    return false;
  }
  return m_parent->hasAncestor(e);
}

void ExpressionLayout::collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) {
}

void ExpressionLayout::addSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling) {
  privateAddSibling(cursor, sibling, false);
}

void ExpressionLayout::addSiblingAndMoveCursor(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling) {
  privateAddSibling(cursor, sibling, true);
}

ExpressionLayout * ExpressionLayout::replaceWith(ExpressionLayout * newChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  m_parent->replaceChild(this, newChild, deleteAfterReplace);
  return newChild;
}

ExpressionLayout * ExpressionLayout::replaceWithAndMoveCursor(ExpressionLayout * newChild, bool deleteAfterReplace, ExpressionLayoutCursor * cursor) {
  assert(m_parent != nullptr);
  m_parent->replaceChildAndMoveCursor(this, newChild, deleteAfterReplace, cursor);
  return newChild;
}

void ExpressionLayout::replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) {
  assert(newChild != nullptr);
  // Caution: handle the case where we replace an operand with a descendant of ours.
  if (newChild->hasAncestor(this)) {
    newChild->editableParent()->detachChild(newChild);
  }
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  for (int i = 0; i < numberOfChildren(); i++) {
    if (op[i] == oldChild) {
      if (oldChild != nullptr && oldChild->parent() == this) {
        const_cast<ExpressionLayout *>(oldChild)->setParent(nullptr);
      }
      if (deleteOldChild) {
        delete oldChild;
      }
      if (newChild != nullptr) {
        const_cast<ExpressionLayout *>(newChild)->setParent(this);
      }
      op[i] = newChild;
      break;
    }
  }
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
}

void ExpressionLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  assert(indexOfChild(oldChild) >= 0);
  if (!newChild->hasAncestor(oldChild)) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  }
  replaceChild(oldChild, newChild, deleteOldChild);
  cursor->setPointedExpressionLayout(newChild);
}

void ExpressionLayout::detachChild(const ExpressionLayout * e) {
  assert(indexOfChild(e) >= 0);
  detachChildAtIndex(indexOfChild(e));
}

void ExpressionLayout::detachChildren() {
  for (int i = 0; i < numberOfChildren(); i++) {
    detachChildAtIndex(i);
  }
}

void ExpressionLayout::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  assert(index >= 0 && index < numberOfChildren());
  replaceChild(editableChild(index), new EmptyLayout(), deleteAfterRemoval);
}

void ExpressionLayout::removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) {
  assert(index >= 0 && index < numberOfChildren());
  assert(cursor->pointedExpressionLayout()->hasAncestor(child(index), true));
  removeChildAtIndex(index, deleteAfterRemoval);
  if (index < numberOfChildren()) {
    cursor->setPointedExpressionLayout(editableChild(index));
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  int indexOfNewPointedLayout = index - 1;
  assert(indexOfNewPointedLayout >= 0);
  assert(indexOfNewPointedLayout < numberOfChildren());
  cursor->setPointedExpressionLayout(editableChild(indexOfNewPointedLayout));
}


bool ExpressionLayout::insertLayoutAtCursor(ExpressionLayout * newChild, ExpressionLayoutCursor * cursor) {
  cursor->pointedExpressionLayout()->addSibling(cursor, newChild);
  return true;
}

void ExpressionLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  int indexOfPointedExpression = indexOfChild(cursor->pointedExpressionLayout());
  if (indexOfPointedExpression >= 0) {
    // Case: The pointed layout is a child. Move Left.
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    bool shouldRecomputeLayout = false;
    cursor->moveLeft(&shouldRecomputeLayout);
    return;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: this is the pointed layout.
  if (m_parent == nullptr) {
    // Case: No parent. Return.
    return;
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    // Case: Left. Ask the parent.
    if (m_parent) {
      m_parent->deleteBeforeCursor(cursor);
    }
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right. Delete the layout.
  m_parent->removePointedChildAtIndexAndMoveCursor(m_parent->indexOfChild(this), true, cursor);
}

char ExpressionLayout::XNTChar() const {
  if (m_parent == nullptr) {
    return 'x';
  }
  return m_parent->XNTChar();
}

bool ExpressionLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  return moveVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

bool ExpressionLayout::moveUpInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveInside(VerticalDirection::Up, cursor, shouldRecomputeLayout);
}

bool ExpressionLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  return moveVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

bool ExpressionLayout::moveDownInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveInside(VerticalDirection::Down, cursor, shouldRecomputeLayout);
}

ExpressionLayoutCursor ExpressionLayout::equivalentCursor(ExpressionLayoutCursor * cursor) {
  // Only HorizontalLayout may not have a parent, and it overload this function
  assert(m_parent);
  if (cursor->pointedExpressionLayout() == this) {
    return m_parent->equivalentCursor(cursor);
  } else {
    return ExpressionLayoutCursor();
  }
}

ExpressionLayout * ExpressionLayout::layoutToPointWhenInserting() {
  if (numberOfChildren() > 0) {
    return editableChild(0);
  }
  return this;
}

bool ExpressionLayout::addGreySquaresToAllMatrixAncestors() {
  bool addedSquares = false;
  ExpressionLayout * currentAncestor = m_parent;
  while (currentAncestor != nullptr) {
    if (currentAncestor->isMatrix()) {
      static_cast<MatrixLayout *>(currentAncestor)->addGreySquares();
      addedSquares = true;
    }
    currentAncestor = currentAncestor->editableParent();
  }
  return addedSquares;
}

bool ExpressionLayout::hasText() const {
  /* A layout has text if it is not empty and it is not an horizontal layout
   * with no child or with one child with no text. */
  return !isEmpty() && !(isHorizontal() && (numberOfChildren() == 0 || (numberOfChildren() == 1 && !child(0)->hasText())));
}

bool ExpressionLayout::canBeOmittedMultiplicationLeftFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true too. If isCollapsable changes, it might not be the
   * case anymore so make sure to modify this function if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, true);
}

bool ExpressionLayout::canBeOmittedMultiplicationRightFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true and isVerticalOffset is false. If one of these
   * functions changes, it might not be the case anymore so make sure to modify
   * canBeOmittedMultiplicationRightFactor if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, false) && !isVerticalOffset();
}

void ExpressionLayout::detachChildAtIndex(int i) {
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  if (op[i] != nullptr && op[i]->parent() == this) {
    const_cast<ExpressionLayout *>(op[i])->setParent(nullptr);
  }
  op[i] = nullptr;
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
}

bool ExpressionLayout::moveInside(VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  ExpressionLayout *  chilResult = nullptr;
  ExpressionLayout ** childResultPtr = &chilResult;
  ExpressionLayoutCursor::Position resultPosition = ExpressionLayoutCursor::Position::Left;
  /* The distance between the cursor and its next position cannot be greater
   * than this initial value of score. */
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  moveCursorInsideAtDirection(direction, cursor, shouldRecomputeLayout, childResultPtr, &resultPosition, &resultScore);

  // If there is a valid result
  if (*childResultPtr == nullptr) {
    return false;
  }
  cursor->setPointedExpressionLayout(*childResultPtr);
  cursor->setPosition(resultPosition);
  *shouldRecomputeLayout = (*childResultPtr)->addGreySquaresToAllMatrixAncestors();
  return true;
}

bool ExpressionLayout::moveVertically(VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (!equivalentPositionVisited) {
    ExpressionLayoutCursor cursorEquivalent = equivalentCursor(cursor);
    if (cursorEquivalent.isDefined()) {
      cursor->setPointedExpressionLayout(cursorEquivalent.pointedExpressionLayout());
      cursor->setPosition(cursorEquivalent.position());
      if (direction == VerticalDirection::Up) {
        return cursor->pointedExpressionLayout()->moveUp(cursor, shouldRecomputeLayout, true);
      } else {
        return cursor->pointedExpressionLayout()->moveDown(cursor, shouldRecomputeLayout, true);
      }
    }
  }
  if (m_parent) {
    if (direction == VerticalDirection::Up) {
        return m_parent->moveUp(cursor, shouldRecomputeLayout, true);
      } else {
        return m_parent->moveDown(cursor, shouldRecomputeLayout, true);
      }
  }
  return false;
}

void ExpressionLayout::moveCursorInsideAtDirection (
    VerticalDirection direction,
    ExpressionLayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    ExpressionLayout ** childResult,
    void * resultPosition,
    int * resultScore)
{
  ExpressionLayoutCursor::Position * castedResultPosition = static_cast<ExpressionLayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction == VerticalDirection::Up ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
  bool layoutContains = m_frame.contains(cursorMiddleLeft);

  if (layoutIsUnderOrAbove) {
    // Check the distance to a Left cursor.
    int currentDistance = ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance <= *resultScore ){
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Left;
      *resultScore = currentDistance;
    }

    // Check the distance to a Right cursor.
    currentDistance = ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance < *resultScore) {
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Right;
      *resultScore = currentDistance;
    }
  }
  if (layoutIsUnderOrAbove || layoutContains) {
    int childIndex = 0;
    while (child(childIndex++)) {
      editableChild(childIndex-1)->moveCursorInsideAtDirection(direction, cursor, shouldRecomputeLayout, childResult, castedResultPosition, resultScore);
    }
  }
}

void ExpressionLayout::privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) {
  /* The layout must have a parent, because HorizontalLayout overrides
   * privateAddSibling and only an HorizontalLayout can be the root layout. */
  assert(m_parent);
  if (m_parent->isHorizontal()) {
    int indexInParent = m_parent->indexOfChild(this);
    int siblingIndex = cursor->position() == ExpressionLayoutCursor::Position::Left ? indexInParent : indexInParent + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. */
    ExpressionLayout * neighbour = nullptr;
    if (cursor->position() == ExpressionLayoutCursor::Position::Left && indexInParent > 0) {
      neighbour = m_parent->editableChild(indexInParent - 1);
    } else if (cursor->position() == ExpressionLayoutCursor::Position::Right && indexInParent < m_parent->numberOfChildren() - 1) {
      neighbour = m_parent->editableChild(indexInParent + 1);
    }
    if (neighbour != nullptr && neighbour->isVerticalOffset()) {
      cursor->setPointedExpressionLayout(neighbour);
      cursor->setPosition(cursor->position() == ExpressionLayoutCursor::Position::Left ? ExpressionLayoutCursor::Position::Right : ExpressionLayoutCursor::Position::Left);
      if (moveCursor) {
        neighbour->addSiblingAndMoveCursor(cursor, sibling);
      } else {
        neighbour->addSibling(cursor, sibling);
      }
      return;
    }

    // Else, let the parent add the sibling.
    if (moveCursor) {
      if (siblingIndex < m_parent->numberOfChildren()) {
        cursor->setPointedExpressionLayout(m_parent->editableChild(siblingIndex));
        cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      } else {
        cursor->setPointedExpressionLayout(m_parent);
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      }
    }
    static_cast<HorizontalLayout *>(m_parent)->addOrMergeChildAtIndex(sibling, siblingIndex, true);
    return;
  }
  ExpressionLayout * juxtapositionLayout = nullptr;
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    juxtapositionLayout = replaceWithJuxtapositionOf(sibling, this, false);
  } else {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
    juxtapositionLayout = replaceWithJuxtapositionOf(this, sibling, false);
  }
  if (moveCursor) {
    cursor->setPointedExpressionLayout(juxtapositionLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  }
}

void ExpressionLayout::collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex) {
  if (!parent() || !parent()->isHorizontal()) {
    return;
  }
  int indexInParent = parent()->indexOfChild(this);
  int numberOfSiblings = parent()->numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  ExpressionLayout * absorbingChild = editableChild(absorbingChildIndex);
  if (!absorbingChild || !absorbingChild->isHorizontal()) {
    return;
  }
  HorizontalLayout * horizontalAbsorbingChild = static_cast<HorizontalLayout *>(absorbingChild);
  if (direction == HorizontalDirection::Right && indexInParent < numberOfSiblings - 1) {
    canCollapse = !(editableParent()->editableChild(indexInParent+1)->mustHaveLeftSibling());
  }
  ExpressionLayout * sibling = nullptr;
  while (canCollapse) {
    if (direction == HorizontalDirection::Right && indexInParent == numberOfSiblings - 1) {
      break;
    }
    if (direction == HorizontalDirection::Left && indexInParent == 0) {
      break;
    }
    int siblingIndex = direction == HorizontalDirection::Right ? indexInParent+1 : indexInParent-1;
    sibling = editableParent()->editableChild(siblingIndex);
    if (sibling->isCollapsable(&numberOfOpenParenthesis, direction == HorizontalDirection::Left)) {
      editableParent()->removeChildAtIndex(siblingIndex, false);
      int newIndex = direction == HorizontalDirection::Right ? absorbingChild->numberOfChildren() : 0;
      horizontalAbsorbingChild->addOrMergeChildAtIndex(sibling, newIndex, true);
      numberOfSiblings--;
      if (direction == HorizontalDirection::Left) {
        indexInParent--;
      }
    } else {
      break;
    }
  }
}

ExpressionLayout * ExpressionLayout::replaceWithJuxtapositionOf(ExpressionLayout * leftChild, ExpressionLayout * rightChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  assert(!m_parent->isHorizontal());
  /* One of the children to juxtapose might be "this", so we first have to
   * replace "this" with an horizontal layout, then add "this" to the layout. */
  ExpressionLayout * layout = new HorizontalLayout();
  m_parent->replaceChild(this, layout, deleteAfterReplace);
  layout->addChildAtIndex(leftChild, 0);
  layout->addChildAtIndex(rightChild, 1);
  return layout;
}

}

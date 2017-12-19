#include <assert.h>
#include <stdlib.h>
#include <poincare/src/layout/editable_string_layout.h>
#include <poincare/src/layout/horizontal_layout.h>
#include <poincare/expression_layout_cursor.h>
#include <ion/display.h>

namespace Poincare {

ExpressionLayout::ExpressionLayout() :
  m_baseline(0),
  m_parent(nullptr),
  m_sized(false),
  m_positioned(false),
  m_frame(KDRectZero) {
}

const ExpressionLayout * const * ExpressionLayout::ExpressionLayoutArray2(const ExpressionLayout * e1, const ExpressionLayout * e2) {
  static const ExpressionLayout * result[2] = {nullptr, nullptr};
  result[0] = e1;
  result[1] = e2;
  return result;
}

const ExpressionLayout * const * ExpressionLayout::ExpressionLayoutArray3(const ExpressionLayout * e1, const ExpressionLayout * e2, const ExpressionLayout * e3) {
  static const ExpressionLayout * result[3] = {nullptr, nullptr, nullptr};
  result[0] = e1;
  result[1] = e2;
  result[2] = e3;
  return result;
}

KDPoint ExpressionLayout::origin() {
  if (m_parent == nullptr) {
    return absoluteOrigin();
  } else {
    return KDPoint(absoluteOrigin().x() - m_parent->absoluteOrigin().x(),
        absoluteOrigin().y() - m_parent->absoluteOrigin().y());
  }
}

void ExpressionLayout::draw(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  int i = 0;
  while (ExpressionLayout * c = editableChild(i++)) {
    c->draw(ctx, p, expressionColor, backgroundColor);
  }
  render(ctx, absoluteOrigin().translatedBy(p), expressionColor, backgroundColor);
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

void ExpressionLayout::invalidAllSizesAndPositions() {
  m_sized = false;
  m_positioned = false;
  for (int i = 0; i < numberOfChildren(); i++) {
    editableChild(i)->invalidAllSizesAndPositions();
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

int ExpressionLayout::indexOfChild(ExpressionLayout * child) const {
  for (int i = 0; i < numberOfChildren(); i++) {
    if (children()[i] == child) {
      return i;
    }
  }
  return -1;
}

void ExpressionLayout::setParent(ExpressionLayout* parent) {
  m_parent = parent;
}

bool ExpressionLayout::hasAncestor(const ExpressionLayout * e) const {
  assert(m_parent != this);
  if (m_parent == e) {
    return true;
  }
  if (m_parent == nullptr) {
    return false;
  }
  return m_parent->hasAncestor(e);
}

void ExpressionLayout::addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) {
  if (m_parent) {
    int brotherIndex = cursor->position() == ExpressionLayoutCursor::Position::Left ? m_parent->indexOfChild(this) : m_parent->indexOfChild(this) + 1;
    if (m_parent->addChildAtIndex(brother, brotherIndex)) {
      return;
    }
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(brother, this, false);
    return;
  }
  replaceWithJuxtapositionOf(this, brother, false);
  //TODO Inside position
}

bool ExpressionLayout::insertLayoutForTextAtCursor(const char * text, ExpressionLayoutCursor * cursor) {
  EditableStringLayout * newChild = new EditableStringLayout(text, strlen(text));
  cursor->pointedExpressionLayout()->addBrother(cursor, newChild);
  cursor->setPointedExpressionLayout(newChild);
  cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  cursor->setPositionInside(0);
  return true;
}

ExpressionLayout * ExpressionLayout::replaceWith(ExpressionLayout * newChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  m_parent->replaceChild(this, newChild, deleteAfterReplace);
  return newChild;
}

ExpressionLayout * ExpressionLayout::replaceWithJuxtapositionOf(ExpressionLayout * leftChild, ExpressionLayout * rightChild, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  /* One of the children to juxtapose might be "this", so we first have to
   * replace "this" with an horizontal layout, then add "this" to the layout. */
  ExpressionLayout * layout = new HorizontalLayout();
  m_parent->replaceChild(this, layout, deleteAfterReplace);
  layout->addChildAtIndex(leftChild, 0);
  layout->addChildAtIndex(rightChild, 1);
  return layout;
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
}

void ExpressionLayout::detachChild(const ExpressionLayout * e) {
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  for (int i = 0; i < numberOfChildren(); i++) {
    if (op[i] == e) {
      detachChildAtIndex(i);
    }
  }
}

void ExpressionLayout::detachChildAtIndex(int i) {
  ExpressionLayout ** op = const_cast<ExpressionLayout **>(children());
  if (op[i] != nullptr && op[i]->parent() == this) {
    const_cast<ExpressionLayout *>(op[i])->setParent(nullptr);
  }
  op[i] = nullptr;
  m_sized = false;
}

bool ExpressionLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  if (m_parent) {
    return m_parent->moveUp(cursor, this, previousLayout);
  }
  return false;
}

bool ExpressionLayout::moveUpInside(ExpressionLayoutCursor * cursor) {
  return moveInside(VerticalDirection::Up, cursor);
}

bool ExpressionLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  if (m_parent) {
    return m_parent->moveDown(cursor, this, previousLayout);
  }
  return false;
}

bool ExpressionLayout::moveDownInside(ExpressionLayoutCursor * cursor) {
  return moveInside(VerticalDirection::Down, cursor);
}

bool ExpressionLayout::moveInside(VerticalDirection direction, ExpressionLayoutCursor * cursor) {
  ExpressionLayout *  chilResult = nullptr;
  ExpressionLayout ** childResultPtr = &chilResult;
  ExpressionLayoutCursor::Position resultPosition = ExpressionLayoutCursor::Position::Left;
  int resultPositionInside = 0;
  // The distance between the cursor and its next position cannot be greater
  // than this initial value of score.
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  moveCursorInsideAtDirection(direction, cursor, childResultPtr, &resultPosition, &resultPositionInside, &resultScore);

  // If there is a valid result
  if (*childResultPtr == nullptr) {
    return false;
  }
  cursor->setPointedExpressionLayout(*childResultPtr);
  cursor->setPosition(resultPosition);
  cursor->setPositionInside(resultPositionInside);
  return true;
}

void ExpressionLayout::moveCursorInsideAtDirection (
    VerticalDirection direction,
    ExpressionLayoutCursor * cursor,
    ExpressionLayout ** childResult,
    void * resultPosition,
    int * resultPositionInside,
    int * resultScore)
{
  ExpressionLayoutCursor::Position * castedResultPosition = static_cast<ExpressionLayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction == VerticalDirection::Up ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
  bool layoutContains = m_frame.contains(cursorMiddleLeft);

  if (layoutIsUnderOrAbove) {
    // Check the distance to a Left cursor.
    int currentDistance = cursor->middleLeftPointOfCursor(this, ExpressionLayoutCursor::Position::Left, 0).squareDistanceTo(cursorMiddleLeft);
    if (currentDistance <= *resultScore ){
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Left;
      *resultPositionInside = 0;
      *resultScore = currentDistance;
    }

    // Check the distance to a Right cursor.
    currentDistance = cursor->middleLeftPointOfCursor(this, ExpressionLayoutCursor::Position::Right, 0).squareDistanceTo(cursorMiddleLeft);
    if (currentDistance < *resultScore) {
      *childResult = this;
      *castedResultPosition = ExpressionLayoutCursor::Position::Right;
      *resultPositionInside = 0;
      *resultScore = currentDistance;
    }
  }
  if (layoutIsUnderOrAbove || layoutContains) {
    int childIndex = 0;
    while (child(childIndex++)) {
      editableChild(childIndex-1)->moveCursorInsideAtDirection(direction, cursor, childResult, castedResultPosition, resultPositionInside, resultScore);
    }
  }
}

}

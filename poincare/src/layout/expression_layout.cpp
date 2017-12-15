#include <assert.h>
#include <stdlib.h>
#include "string_layout.h"
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

KDCoordinate ExpressionLayout::baseline() {
  return m_baseline;
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
  while (ExpressionLayout * c = child(i++)) {
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

void ExpressionLayout::setParent(ExpressionLayout* parent) {
  m_parent = parent;
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
      child(childIndex-1)->moveCursorInsideAtDirection(direction, cursor, childResult, castedResultPosition, resultPositionInside, resultScore);
    }
  }
}

}

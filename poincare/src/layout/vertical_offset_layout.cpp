#include "vertical_offset_layout.h"
#include "empty_layout.h"
#include "parenthesis_left_layout.h"
#include "parenthesis_right_layout.h"
#include <ion/charset.h>
#include <poincare/expression_layout_cursor.h>
#include <poincare/layout_engine.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

VerticalOffsetLayout::VerticalOffsetLayout(ExpressionLayout * indice, Type type, bool cloneOperands) :
  StaticLayoutHierarchy(indice, cloneOperands),
  m_type(type)
{
}

ExpressionLayout * VerticalOffsetLayout::clone() const {
  VerticalOffsetLayout * layout = new VerticalOffsetLayout(const_cast<VerticalOffsetLayout *>(this)->indiceLayout(), m_type, true);
  return layout;
}

void VerticalOffsetLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == indiceLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    ExpressionLayout * base = baseLayout();
    if (indiceLayout()->isEmpty()) {
      int indexInParent = m_parent->indexOfChild(this);
      if (base->isEmpty()) {
        // Case: Empty base and indice.
        // Remove both the base and the indice layout.
        ExpressionLayout * parent = m_parent;
        cursor->setPointedExpressionLayout(this);
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        parent->removePointedChildAtIndexAndMoveCursor(indexInParent, true, cursor);
        cursor->setPointedExpressionLayout(parent->editableChild(indexInParent-1));
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        parent->removePointedChildAtIndexAndMoveCursor(indexInParent-1, true, cursor);
        return;
      }
      // Case: Empty indice only.
      // Delete the layout.
      cursor->setPointedExpressionLayout(base);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      m_parent->removeChildAtIndex(indexInParent, true);
      return;
    }
    // Case: Non-empty indice.
    // Move Left of the VerticalOffsetLayout.
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    // Case: Right.
    // Move to the indice.
    cursor->setPointedExpressionLayout(indiceLayout());
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool VerticalOffsetLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the indice.
  // Go Left.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the indice.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
    return true;
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool VerticalOffsetLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the indice.
  // Go Right.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the indice.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
    return true;
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool VerticalOffsetLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: Superscript.
  if (m_type == VerticalOffsetLayout::Type::Superscript) {
    // Case: Right.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    // Case: Left.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Left)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
  }
  // Case: Subscript.
  // Case: Left or Right of the indice.
  // Put the cursor at the same position, pointing this.
  if (m_type == VerticalOffsetLayout::Type::Subscript
    && indiceLayout() != nullptr
    && (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Left)
      || cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Right)))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

bool VerticalOffsetLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: Subscript.
  if (m_type == VerticalOffsetLayout::Type::Subscript) {
    // Case: Right.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    // Case: Left.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Left)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
  }
  // Case: Superscript.
  // Case: Left or Right of the indice.
  // Put the cursor at the same position, pointing this.
  if (m_type == VerticalOffsetLayout::Type::Superscript
    && indiceLayout() != nullptr
    && cursor->pointedExpressionLayout() == indiceLayout())
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

int VerticalOffsetLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (m_type == Type::Subscript) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
      return 0;
    }
    // If the layout is a subscript, write "_{indice}"
    int numberOfChar = LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '_');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '{');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += const_cast<VerticalOffsetLayout *>(this)->indiceLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '}');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    return numberOfChar;
  }
  assert(m_type == Type::Superscript);
  // If the layout is a superscript, write "^(indice)"
  int numberOfChar = LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "^");

  // Add a multiplication if omitted.
  int indexInParent = -1;
  if (m_parent) {
    indexInParent = m_parent->indexOfChild(this);
  }
  if (indexInParent >= 0 && indexInParent < (m_parent->numberOfChildren() - 1) && m_parent->isHorizontal() && m_parent->child(indexInParent + 1)->canBeOmittedMultiplicationRightFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

ExpressionLayout * VerticalOffsetLayout::indiceLayout() {
  return editableChild(0);
}

ExpressionLayout * VerticalOffsetLayout::baseLayout() {
  int indexInParent = parent()->indexOfChild(this);
  assert(indexInParent > 0);
  return editableParent()->editableChild(indexInParent - 1);
}

void VerticalOffsetLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a subscript/superscript, only the position of the child matters
}

KDSize VerticalOffsetLayout::computeSize() {
  KDSize indiceSize = indiceLayout()->size();
  KDCoordinate width = indiceSize.width();
  if (m_type == Type::Superscript) {
    assert(m_parent != nullptr);
    assert(m_parent->isHorizontal());
    int indexInParent = m_parent->indexOfChild(this);
    if (indexInParent < m_parent-> numberOfChildren() - 1 && m_parent->editableChild(indexInParent + 1)->hasUpperLeftIndex()) {
      width += 5;
    }
  }
  KDCoordinate height = 0;
  if (m_type == Type::Subscript) {
    height = positionOfChild(indiceLayout()).y()+ indiceLayout()->size().height();
  } else {
    height = indiceLayout()->size().height() + baseLayout()->baseline() - k_indiceHeight;
  }
  return KDSize(width, height);
}

void VerticalOffsetLayout::computeBaseline() {
  if (m_type == Type::Subscript) {
    m_baseline = 0;
  } else {
    m_baseline = size().height();
  }
  m_baselined = true;
}

KDPoint VerticalOffsetLayout::positionOfChild(ExpressionLayout * child) {
  assert(child == indiceLayout());
  if (m_type == Type::Superscript) {
    return KDPointZero;
  }
  assert(m_type == Type::Subscript);
  ExpressionLayout * base = baseLayout();
  return KDPoint(0, base->size().height() - base->baseline() - k_indiceHeight);
}

void VerticalOffsetLayout::privateAddBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother, bool moveCursor) {
  if (brother->isVerticalOffset()){
    VerticalOffsetLayout * verticalOffsetBrother = static_cast<VerticalOffsetLayout *>(brother);
    if (verticalOffsetBrother->type() == Type::Superscript) {
      // Add parenthesis
      assert(m_parent->isHorizontal());
      int indexInParent = m_parent->indexOfChild(this);
      // Add the right parenthesis
      ParenthesisRightLayout * parenthesisRight = new ParenthesisRightLayout();
      m_parent->addChildAtIndex(parenthesisRight, indexInParent + 1);
      // Add the left parenthesis
      ParenthesisLeftLayout * parenthesisLeft = new ParenthesisLeftLayout();
      int leftParenthesisIndex = indexInParent;
      int numberOfOpenParenthesis = 0;
      while (leftParenthesisIndex > 0 && editableParent()->editableChild(leftParenthesisIndex-1)->isCollapsable(&numberOfOpenParenthesis,true)) {
        leftParenthesisIndex--;
      }
      m_parent->addChildAtIndex(parenthesisLeft, leftParenthesisIndex);
      if (moveCursor) {
        parenthesisRight->addBrotherAndMoveCursor(cursor, brother);
      } else {
        parenthesisRight->addBrother(cursor, brother);
      }
      return;
    }
  }
  ExpressionLayout::privateAddBrother(cursor, brother, moveCursor);
}


}
